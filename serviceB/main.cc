#include <iostream>
#include <string>
#include <memory>
#include <thread>

#include <grpc++/grpc++.h>
#include "do_work.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using workservice::grpc::WorkService;
using workservice::grpc::WorkServiceReply;
using workservice::grpc::WorkServiceRequest;

class WorkServiceImpl final : public WorkService::Service {
    Status DoWork(ServerContext* context, const WorkServiceRequest* request, WorkServiceReply* reply) override {
        std::cout << "Service B received request\n"
                  << "Doing work for " << std::to_string(request->duration_ms()) << "ms ..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(request->duration_ms()));
        std::cout << "Service B finished work\n";
        reply->set_message("Service B worked for " + std::to_string(request->duration_ms()));
        return Status::OK;
    }
};

void RunServer() {
    std::string     server_address("0.0.0.0:3001");
    WorkServiceImpl service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);

    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Service B listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main() {
    std::cout << "Hello, from service B!" << std::endl;
    RunServer();
    return 0;
}
