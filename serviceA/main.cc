#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>

#include <grpc++/grpc++.h>
#include <absl/strings/str_format.h>

#include "do_work.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using workservice::grpc::WorkService;
using workservice::grpc::WorkServiceReply;
using workservice::grpc::WorkServiceRequest;

class WorkServiceClient {
public:
    WorkServiceClient(std::shared_ptr<Channel> channel) : stub_(WorkService::NewStub(channel)) {
    }

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string DoWork(uint32_t random) {
        // Data we are sending to the server.
        WorkServiceRequest request;
        request.set_duration_ms(random);
        // Container for the data we expect from the server.
        WorkServiceReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->DoWork(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<WorkService::Stub> stub_;
};

int main(int argc, char** argv) {
    int         service_id = 0;
    std::string service_b_ip, service_c_ip;
#ifndef REMOTE_SERVICES
    service_b_ip = "0.0.0.0";
    service_c_ip = "0.0.0.0";
#else
    service_b_ip = "service-b";
    service_c_ip = "service-c";
#endif
    while (1) {
        std::cout << "Hello, from client service A!\n";
        std::cout << "Send work to be done to:\n";
        std::cout << "1. Server service B\n";
        std::cout << "2. Server service C\n";
        std::cin >> service_id;
        srand(time(NULL));
        uint32_t random = 1000 + (rand() % 5000);
        if (service_id == 1) {
            WorkServiceClient wsClient(
                grpc::CreateChannel(absl::StrFormat("%s:3001", service_b_ip), grpc::InsecureChannelCredentials()));
            std::cout << wsClient.DoWork(random) << std::endl;

        } else if (service_id == 2) {
            WorkServiceClient wsClient2(
                grpc::CreateChannel(absl::StrFormat("%s:3002", service_c_ip), grpc::InsecureChannelCredentials()));
            std::cout << wsClient2.DoWork(random) << std::endl;
        } else {
            std::cerr << "Invalid service id: " << service_id << std::endl;
        }
    }

    return 0;
}
