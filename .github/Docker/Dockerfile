FROM ubuntu:22.04 AS mongo-driver-builder-stage

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    build-essential \
    libssl-dev \
    python3 \
 && rm -rf /var/lib/apt/lists/*

ARG MONGO_CXX_DRIVER_VERSION=r3.10.0 # Or your desired version
WORKDIR /tmp
RUN git clone --depth 1 --branch ${MONGO_CXX_DRIVER_VERSION} https://github.com/mongodb/mongo-cxx-driver.git mongo-cxx-driver \
 && cd mongo-cxx-driver \
 && cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_CXX_STANDARD=20 \
    -DBSONCXX_POLY_USE_MNMLSTC_STRING_VIEW=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DENABLE_TESTS=OFF \
 && cmake --build build --parallel $(nproc) \
 && cmake --install build \
 && cd /tmp \
 && rm -rf mongo-cxx-driver

# This image now contains the built driver in /usr/local
# You can add a dummy CMD if needed, but it's mainly for copying from.
CMD ["echo", "MongoDB C++ Driver built and installed in /usr/local"]
