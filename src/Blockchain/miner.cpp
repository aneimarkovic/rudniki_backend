//
// Created by Anei Markovic on 1/5/26.
//
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <mpi.h>

// Mapped Windows VS POSIX
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    typedef int ssize_t;
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #define CLOSE_SOCKET close
#endif

#include "Blockchain.h"
#include "Structs/Consts.h"

using namespace std;

#define TAG_BLOCK 1
#define TAG_CONTROL 2
#define CMD_START 1
#define CMD_STOP 2
#define CMD_EXIT 3

atomic<bool> g_isMining(false);
atomic<bool> g_blockFound(false);
atomic<bool> g_programRunning(true);
atomic<long> g_foundNonce(0);
atomic<int> g_nextCmd(0);

string g_targetData = "";
mutex g_dataMutex;

// Funkcija za rudarjenje z izboljšanim korakom (stride)
void minerThread(int id, int step, Block b, int difficulty) {
    long nonce = id;
    string target(difficulty, '0');

    while(g_isMining && !g_blockFound) {
        b.nonce = nonce;
        string h = Block::calculateHash(b);
        if (h.substr(0, difficulty) == target) {
            if (!g_blockFound.exchange(true)) {
                g_foundNonce = b.nonce;
            }
            return;
        }
        nonce += step;
        if (nonce % 1000 == 0) this_thread::yield();
    }
}

void webServer(Blockchain* bc) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;
#endif

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address); // Changed int to socklen_t

    // Strežnik na portu 8081
    if ((server_fd = ::socket(AF_INET, SOCK_STREAM, 0)) == 0) return;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))) return;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(8081);

    if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) return;
    if (::listen(server_fd, 3) < 0) return;

    while(g_programRunning) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        struct timeval tv = {1, 0};

        if (select(server_fd + 1, &readfds, NULL, NULL, &tv) > 0) {
            if ((new_socket = ::accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) continue;

            char buffer[1024] = { 0 };
            ssize_t bytesRead = recv(new_socket, buffer, 1024, 0);
            if (bytesRead <= 0) {
                CLOSE_SOCKET(new_socket);
                continue;
            }
            string request(buffer);
            stringstream response;

            std::cout << "REQUIEST: " << request << std::endl;

            size_t minePos = request.find("GET mine/");
            std::cout << "minePos: " << minePos << std::endl;
            if (minePos != string::npos) {
                size_t endPos = request.find(" ", minePos + 9);
                std::cout << "endPos: " << endPos << std::endl;
                string data = request.substr(minePos + 9, endPos - (minePos + 9));

                std::cout << "data: " << data << std::endl;
                {
                    lock_guard<mutex> lock(g_dataMutex);
                    g_targetData = data;
                }
                g_nextCmd = CMD_START;

                response << "HTTP/1.1 200 OK\r\n";
                response << "Connection: close\r\n";
                response << "Content-Type: text/plain\r\n";
                response << "\r\n";
                response << "Mining started for: " << data;
            } else {
                vector<Block> chain = bc->getChain();

                response << "HTTP/1.1 200 OK\r\n";
                response << "Connection: close\r\n";
                response << "Content-Type: text/html\r\n";
                response << "\r\n";

                response << "<html><body><h1>Blockchain MPI Cluster</h1>";
                response << "<p>To mine: /mine/YOUR_DATA</p>";
                response << "<ul>";
                for(const auto& b : chain) {
                    response << "<li><b>Block " << b.index << "</b>: " << b.data << " (" << b.currentBlockHash << ") [difficulty: "<< b.difficulty <<"]</li>";
                }
                response << "</ul></body></html>";
            }

            string resp = response.str();
            send(new_socket, resp.c_str(), resp.length(), 0);
            CLOSE_SOCKET(new_socket);
        }
    }
    CLOSE_SOCKET(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
}


void inputListener() {
    string line;
    cout << "\nCommands: mine, stop, print, exit" << endl;
    while(g_programRunning) {
        cout << "> ";
        if (!getline(cin, line)) break;

        if (line == "mine") {
            {
                lock_guard<mutex> lock(g_dataMutex);
                g_targetData = "Default Console Data";
            }
            g_nextCmd = CMD_START;
        }
        else if (line == "stop") g_nextCmd = CMD_STOP;
        else if (line == "exit") g_nextCmd = CMD_EXIT;
        else if (line == "print") g_nextCmd = 4;
        else cout << "Unknown command." << endl;
    }
}

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    Blockchain bc;

    thread webThread;
    // thread inputThread;

    if (world_rank == 0) {
        webThread = thread(webServer, &bc);
        // inputThread = thread(inputListener);
    }

    char buffer[4096];

    string currentData = "";

    while(g_programRunning) {
        if (world_rank == 0) {
            int cmd = g_nextCmd.exchange(0);

            if (cmd == CMD_EXIT) {
                g_programRunning = false;
                g_isMining = false;
                int c = CMD_EXIT;
                for(int i=1; i<world_size; i++) MPI_Send(&c, 1, MPI_INT, i, TAG_CONTROL, MPI_COMM_WORLD);
                break;
            }
            if (cmd == CMD_START && !g_isMining) {
                {
                    lock_guard<mutex> lock(g_dataMutex);
                    currentData = g_targetData;
                }

                g_isMining = true;
                int c = CMD_START;
                int len = currentData.length();

                int targetIndex = bc.getLastBlock().index + 1;

                for(int i=1; i<world_size; i++) {
                    MPI_Send(&c, 1, MPI_INT, i, TAG_CONTROL, MPI_COMM_WORLD);
                    MPI_Send(&len, 1, MPI_INT, i, TAG_CONTROL, MPI_COMM_WORLD);
                    MPI_Send(currentData.c_str(), len, MPI_CHAR, i, TAG_CONTROL, MPI_COMM_WORLD);
                    MPI_Send(&targetIndex, 1, MPI_INT, i, TAG_CONTROL, MPI_COMM_WORLD);
                }
                cout << "[MAIN] Mining started for Block " << targetIndex << ". Data: " << currentData << endl;
            }
            if (cmd == CMD_STOP && g_isMining) {
                g_isMining = false;
                int c = CMD_STOP;
                for(int i=1; i<world_size; i++) MPI_Send(&c, 1, MPI_INT, i, TAG_CONTROL, MPI_COMM_WORLD);
                cout << "[MAIN] Mining stopped." << endl;
            }
            if (cmd == 4) {
                vector<Block> chain = bc.getChain();
                for(auto& b : chain) cout << b.toString();
            }
        }

        // DELAVEC
        int flag = 0;
        MPI_Status status;

        MPI_Iprobe(MPI_ANY_SOURCE, TAG_CONTROL, MPI_COMM_WORLD, &flag, &status);
        if (flag) {
            int cmd;
            MPI_Recv(&cmd, 1, MPI_INT, MPI_ANY_SOURCE, TAG_CONTROL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (cmd == CMD_START) {
                if (world_rank != 0) {
                    int len;
                    MPI_Recv(&len, 1, MPI_INT, 0, TAG_CONTROL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    vector<char> d(len + 1);
                    MPI_Recv(d.data(), len, MPI_CHAR, 0, TAG_CONTROL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    d[len] = '\0';
                    currentData = string(d.data());

                    int targetIndex;
                    MPI_Recv(&targetIndex, 1, MPI_INT, 0, TAG_CONTROL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    int myNextIndex = bc.getLastBlock().index + 1;
                    if (myNextIndex > targetIndex) {
                        cout << "[NODE " << world_rank << "] Ignoring stale START command (Target: " << targetIndex << ", I have: " << myNextIndex-1 << ")" << endl;
                        g_isMining = false;
                    } else {
                        g_isMining = true;
                        cout << "[NODE " << world_rank << "] Working on: " << currentData << " (Target: " << targetIndex << ")" << endl;
                    }
                }
            }
            else if (cmd == CMD_STOP) {
                g_isMining = false;
                if (world_rank != 0) cout << "[NODE " << world_rank << "] Stopping..." << endl;
            }
            else if (cmd == CMD_EXIT) {
                g_programRunning = false;
                g_isMining = false;
            }
        }

        MPI_Iprobe(MPI_ANY_SOURCE, TAG_BLOCK, MPI_COMM_WORLD, &flag, &status);
        if (flag) {
            MPI_Recv(buffer, 4096, MPI_CHAR, MPI_ANY_SOURCE, TAG_BLOCK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Block b = Block::deserialize(buffer);
            if (bc.addBlock(b)) {
                cout << "[NODE " << world_rank << "] Accepted Block " << b.index << " from Node " << status.MPI_SOURCE << endl;
                g_blockFound = true;
                g_isMining = false;
            }
        }

        if (g_isMining) {
            Block lastBlock = bc.getLastBlock();
            Block newBlock;
            newBlock.index = lastBlock.index + 1;
            newBlock.timestamp = time(nullptr);

            newBlock.data = currentData;

            newBlock.previousBlockHash = lastBlock.currentBlockHash;
            newBlock.difficulty = bc.getNextDifficulty();

            g_blockFound = false;

            uint16_t numThreads = thread::hardware_concurrency();
            if(numThreads > 2){
                numThreads--;
            }
            // if (numThreads == 0) numThreads = 2;

            // --- OPTIMIZACIJA ZA GRAFE POHITRITVE ---
            int totalRanks = world_size;
            int totalThreads = totalRanks * numThreads;

            vector<thread> threads;
            for(int i = 0; i < numThreads; i++) {
                // Vsaka nit na vsakem računalniku dobi svoj unikaten 'start' in skače za 'totalThreads'
                int globalThreadId = (world_rank * numThreads) + i;
                threads.emplace_back(minerThread, globalThreadId, totalThreads, newBlock, newBlock.difficulty);
            }
            // ----------------------------------------

            while (!g_blockFound && g_isMining) {
                MPI_Iprobe(MPI_ANY_SOURCE, TAG_BLOCK, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                if (flag) g_blockFound = true;

                MPI_Iprobe(MPI_ANY_SOURCE, TAG_CONTROL, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                if (flag) g_blockFound = true;

                this_thread::sleep_for(chrono::milliseconds(10));
            }

            for(auto& t : threads) {
                 if(t.joinable()) t.join();
            }

            if (g_isMining && g_foundNonce != 0 && newBlock.index == bc.getLastBlock().index + 1) {
                newBlock.nonce = g_foundNonce;
                newBlock.currentBlockHash = Block::calculateHash(newBlock);
                if (bc.addBlock(newBlock)) {
                    cout << "[NODE " << world_rank << "] MINED Block " << newBlock.index << " (" << newBlock.data << ") [diff: " << newBlock.difficulty << "]" << endl;
                    string s = newBlock.serialize();
                    for(int i=0; i<world_size; i++) {
                        if (i != world_rank) {
                            MPI_Send(s.c_str(), s.length() + 1, MPI_CHAR, i, TAG_BLOCK, MPI_COMM_WORLD);
                        }
                    }
                    g_isMining = false;
                }
                g_foundNonce = 0;
            }
        } else {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }

    if (world_rank == 0) {
        // inputThread.detach();
        webThread.detach();
    }

    MPI_Finalize();
    return 0;
}
