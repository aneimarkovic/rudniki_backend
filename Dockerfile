FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libboost-all-dev \
    curl \
    wget \
    unzip \
    pkg-config \
    libmongocxx-dev \
    libbsoncxx-dev

WORKDIR /app

COPY . .

RUN mkdir -p build && cd build && \
    cmake .. && \
    make

CMD ["./build/rudniki_backend"]