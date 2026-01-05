//
// Created by Anei Markovic on 1/4/26.
//
#ifndef RUDNIKI_BACKEND_BLOCKCHAIN_H
#define RUDNIKI_BACKEND_BLOCKCHAIN_H
#include <iostream>
#include <string>
#include <vector>
#include <mutex>

using namespace std;

#include <Blockchain/Structs/Consts.h>

class Blockchain {
private:
    vector<Block> chain;
    mutex chainMutex;
public:
    Blockchain() {
        Block genesis;
        genesis.index = 0;
        genesis.timestamp = time(nullptr);
        genesis.data = "Genesis";
        genesis.previousBlockHash = string(64, '0');
        genesis.difficulty = 5;
        genesis.nonce = 0;
        genesis.currentBlockHash = Block::calculateHash(genesis);
        chain.push_back(genesis);
    }

    Block getLastBlock() {
        lock_guard<mutex> lock(chainMutex);
        return chain.back();
    }

    vector<Block> getChain() {
        lock_guard<mutex> lock(chainMutex);
        return chain;
    }

    int getNextDifficulty() {
        lock_guard<mutex> lock(chainMutex);
        if (chain.empty()) return 5;
        Block last = chain.back();
        if (chain.size() < 10) return 5;
        if (chain.size() % 10 != 0) return last.difficulty;

        Block adjustmentBlock = chain[chain.size() - 10];
        long expectedTime = 10 * 10;
        long actualTime = last.timestamp - adjustmentBlock.timestamp;

        if (actualTime < expectedTime / 2) return last.difficulty + 1;
        if (actualTime > expectedTime * 2) return (last.difficulty > 1) ? last.difficulty - 1 : 1;
        return last.difficulty;
    }

    bool addBlock(Block b) {
        lock_guard<mutex> lock(chainMutex);
        if (chain.empty()) return false;
        Block last = chain.back();

        if (b.index <= last.index) return false;
        if (b.index != last.index + 1) return false;
        if (b.previousBlockHash != last.currentBlockHash) return false;

        long currentTime = time(nullptr);
        if (b.timestamp > currentTime + 60) return false;
        if (b.timestamp < last.timestamp - 60) return false;

        string target(b.difficulty, '0');
        if (b.currentBlockHash.substr(0, b.difficulty) != target) return false;

        string recalc = Block::calculateHash(b);
        if (recalc != b.currentBlockHash) return false;

        chain.push_back(b);
        return true;
    }
};
#endif //RUDNIKI_BACKEND_BLOCKCHAIN_H