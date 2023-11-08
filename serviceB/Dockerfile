FROM grpc-proj-base

WORKDIR /app/serviceB
COPY . .

WORKDIR /app

RUN cmake -S . -B build -DBUILD_SERVICE=B -DEXTERNAL_DEPENDENCIES=/app_depends \
  -DEXTERNAL_GRPC_PATH=/app_depends/lib/cmake/grpc \
  -DEXTERNAL_PROTOBUF_PATH=/app_depends/lib/cmake/protobuf \
  -DEXTERNAL_ABSL_PATH=/app_depends/lib/cmake/absl \
  -DEXTERNAL_UTF8_PATH=/app_depends/lib/cmake/utf8_range \
  -DREMOTE_SERVICES=ON

RUN cmake --build build

EXPOSE 3001

CMD ./build/serviceB/service_b
