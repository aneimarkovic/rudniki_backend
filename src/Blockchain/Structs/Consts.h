//
// Created by Anei Markovic on 1/4/26.
//

#ifndef RUDNIKI_BACKEND_CONSTS_H
#define RUDNIKI_BACKEND_CONSTS_H
#include <cstdint>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <Blockchain/Sha256.h>

using namespace std;

struct Block {
    int index;
    string data;
    long long timestamp;
    string previousBlockHash;
    int difficulty;
    long nonce;
    string currentBlockHash;

    string serialize() const {
        return to_string(index) + "|" + to_string(timestamp) + "|" + data + "|" + previousBlockHash + "|" + to_string(difficulty) + "|" + to_string(nonce) + "|" + currentBlockHash;
    }

    static Block deserialize(const string& s) {
        Block b;
        stringstream ss(s);
        string segment;
        vector<string> parts;
        while(getline(ss, segment, '|')) parts.push_back(segment);
        if(parts.size() < 7) { b.index = -1; return b; }
        b.index = stoi(parts[0]);
        b.timestamp = stoll(parts[1]);
        b.data = parts[2];
        b.previousBlockHash = parts[3];
        b.difficulty = stoi(parts[4]);
        b.nonce = stol(parts[5]);
        b.currentBlockHash = parts[6];
        return b;
    }

    static string calculateHash(const Block& block) {
        SHA256 sha;
        stringstream ss;
        ss << block.index << block.data << block.timestamp << block.previousBlockHash << block.difficulty << block.nonce;
        sha.update(ss.str());
        return sha.final();
    }

    string toString() const {
        return "\n--- BLOCK " + to_string(index) + " ---\n" +
               "Data: " + data + "\n" +
               "Hash: " + currentBlockHash + "\n" +
               "Diff: " + to_string(difficulty) + "\n" +
               "Nonce: " + to_string(nonce) + "\n";
    }
};

const uint32_t SHA256::k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};



#endif //RUDNIKI_BACKEND_CONSTS_H