/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <string>
#include <gflags/gflags.h>
#include <grpcpp/grpcpp.h>
#include <chrono>
#include "flat.grpc.pb.h"
#include "util.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
// 端口号
DEFINE_int32(port, 50051, "TCP Port of this server");
// 响应长度
DEFINE_int32(response_len,1024,"Length of request");
// 服务器地址
DEFINE_string(ip_address,"192.168.0.204","The address of server");
test::FlatLogFileFlushChunkResponsePB response;
// Logic and data behind the server's behavior.
class TestServiceImpl final : public test::Test::Service {
  Status Echo(ServerContext* context, const test::FlatLogFileFlushChunkRequestPB* request,
                  test::FlatLogFileFlushChunkResponsePB* reply) override {



    *reply = response;
    return Status::OK;
  }
};

void RunServer(uint16_t port) {
//   std::string server_address = absl::StrFormat("0.0.0.0:%d", port);

  // 服务器地址
  std::string serverAddr = FLAGS_ip_address + ":" + std::to_string(port);

  // 定义服务对象
  TestServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(serverAddr, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << serverAddr << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  // Parse gflags. We recommend you to use gflags as well.
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  response = get_flat_resp(FLAGS_response_len-52);
  std::cout << FLAGS_port << " " << std::endl;
  std::cout << FLAGS_response_len << " " << std::endl;
  printf("the size of message sent is %d\n", response.ByteSize());
  RunServer(FLAGS_port);
  return 0;
}