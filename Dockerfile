FROM mongo-cxx-driver-image:r3.10.0 AS prebuilt_mongo_driver
FROM catch2-image:v3.6.0 AS prebuilt_catch2

FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libssl-dev \
    python3 \
    libboost-dev \
    libboost-system-dev \
    libsodium-dev
 && rm -rf /var/lib/apt/lists/*

COPY --from=prebuilt_mongo_driver /usr/local /usr/local
COPY --from=prebuilt_catch2 /usr/local /usr/local

WORKDIR /app
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build --parallel $(nproc)

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libssl3 \
    libboost-system1.74.0 \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder app/build/src/rudnik_app .

COPY --from=prebuilt_mongo_driver /usr/local/lib/libmongocxx.so* /usr/local/lib/
COPY --from=prebuilt_mongo_driver /usr/local/lib/libbsoncxx.so* /usr/local/lib/
COPY --from=builder /usr/local/lib/libmongoc-1.0.so* /usr/local/lib/
COPY --from=builder /usr/local/lib/libbson-1.0.so* /usr/local/lib/

ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

EXPOSE 8080
CMD ["./rudnik_app"]
