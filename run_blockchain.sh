#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

echo "RUNNING CMAKE"
cmake ..

echo "COMPILING BLOCKCHAIN"
OS=$(uname -s)
if [ "$OS" = "Linux" ]; then
    CORES=$(nproc)
elif [ "$OS" = "Darwin" ]; then
    CORES=$(sysctl -n hw.logicalcpu)
else
    CORES=1
fi

cmake --build . --target blockchain_miner -- -j$CORES

if [ $? -ne 0 ]; then
    echo "ERROR COMPILING FAILED"
    exit 1
fi

echo "KILLING ALL OLD BLOCKCHAIN PROCESSES"
killall blockchain_miner

echo "STARTING NEW BLOCKCHAIN"
nohup mpirun -np 4 ./blockchain_miner > miner.log 2>&1 &

echo "BLOCKCHAIN MINING SERVICE STARTED AT: 127.0.0.1:8081"
sleep 3