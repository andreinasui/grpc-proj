FROM debian:12

RUN apt update && apt install -y \
  g++ \
  build-essential \
  autoconf libtool pkg-config \
  cmake \
  git \
  ninja-build \
  sudo 

WORKDIR /app_depends

RUN git clone --recurse-submodules -b v1.58.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc
RUN git clone --recurse-submodules -b v1.12.0 --depth 1 --shallow-submodules https://github.com/open-telemetry/opentelemetry-cpp
WORKDIR /app_depends/grpc
RUN cmake -S . -B cmake/build -GNinja -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/app_depends
RUN cmake --build cmake/build --target install -- -j$(nproc) 
WORKDIR /app_depends/opentelemetry-cpp
RUN cmake -S . -B cmake/build -GNinja -DWITH_OTLP_GRPC=ON -DWITH_ABSEIL=ON -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=/app_depends -DCMAKE_PREFIX_PATH=/app_depends
RUN cmake --build cmake/build --target install -- -j$(nproc) 

WORKDIR /app_depends
RUN rm -rf grpc opentelemetry-cpp

WORKDIR /app
COPY CMakeLists.txt .
RUN mkdir protos
COPY protos protos
