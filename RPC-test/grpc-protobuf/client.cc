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
#include <memory>
#include <string>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <gflags/gflags.h>
#include "flat.grpc.pb.h"
#include "util.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// 请求长度
DEFINE_int32(request_len,1024,"Length of request");
// 端口号
DEFINE_int32(port,50051,"TCP Port of this server");
// 每个进程的请求次数
DEFINE_uint32(total_request, 10000, "the number of request per processes\n");
// 日志文件
DEFINE_string(log_file, "latency_test.txt", "the number of request per processes\n");
test::FlatLogFileFlushChunkRequestPB request;
test::FlatLogFileFlushChunkResponsePB reply;

std::ofstream outfile;
class Client {
 public:
  Client(std::shared_ptr<Channel> channel)
      : stub_(test::Test::NewStub(channel)) {}


    // SEND A REQUEST
  void SendRequest() {
    // Data we are sending to the server.

    // Container for the data we expect from the server.

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->Echo(&context, request, &reply);
    // Act upon its status.
    if (status.ok()) {
        // std::cout << "ok" << std::endl;
        // std::cout << reply.trace_data() << std::endl;
        return;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
    //   return "RPC failed";
      return;
    }
  }

 private:
  std::unique_ptr<test::Test::Stub> stub_;
};
typedef std::chrono::nanoseconds Nanoseconds;
double _50lat(std::vector<double>& temp) { return temp[uint32_t(0.5 * temp.size())]; }
double _90lat(std::vector<double>& temp) { return temp[uint32_t(0.9 * temp.size())]; }
double _95lat(std::vector<double>& temp) { return temp[uint32_t(0.95 * temp.size())]; }
double _99lat(std::vector<double>& temp) { return temp[uint32_t(0.99 * temp.size())]; }
double _avglat(std::vector<double>& temp) {
    double sum = 0;
    for (auto lat : temp) {
        sum += lat;
    }
    return sum / temp.size();
}
int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    std::string target_str = "192.168.0.204:" + std::to_string(FLAGS_port);

    Client client(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    

      // 写入日志
    outfile.open(FLAGS_log_file+ std::to_string(FLAGS_request_len)+"mtu4096");
     // 存储每次请求的时间
    std::vector<double> latency;
    request = get_flat_req((FLAGS_request_len-352)/3+1);
    // printf("the size of message sent is %d\n", request.ByteSize());

    for (int i = 0; i < FLAGS_total_request+1; ++i) {
        // 开始时间
        auto start = std::chrono::system_clock::now(); // 开始

        // 发送 gRPC 请求
        client.SendRequest();
        // std::cout << i << std::endl;

        // 结束时间
        auto end = std::chrono::system_clock::now();
        
        // 计算并保存请求的持续时间
        auto duration = std::chrono::duration_cast<Nanoseconds>(end-start);
        if(i)latency.push_back(double(duration.count()));
    }
    sort(latency.begin(), latency.end());
    
    outfile <<"total records = " << latency.size() << std::endl;
    outfile <<"request size  = " << request.ByteSize() << std::endl;
    outfile <<"total latency = " << std::endl;
    outfile <<"\tavge latency = "  << _avglat(latency)/1000 << "    us" << std::endl;
    outfile <<"\t50th latency = " << _50lat(latency)/1000 <<"    us" << std::endl;
    outfile <<"\t90th latency = " << _90lat(latency)/1000 <<"    us" << std::endl;
    outfile <<"\t95th latency = " << _95lat(latency)/1000 <<"    us" << std::endl;
    outfile <<"\t99th latency = " << _99lat(latency)/1000 <<"    us" << std::endl;
    outfile.close();
    // std::cout << "why\n";
    exit(0);
    // std::cout << latency.size() << std::endl;






    // double averageTime = totalTime.count() / FLAGS_total_request;
    // std::cout << "Average time for sending a gRPC request: " << averageTime << " microseconds" << std::endl;



//   // 开始时间
//   auto start = std::chrono::high_resolution_clock::now();

//   client.SendRequest(); 
// //   std::cout << "Greeter received: " << reply << std::endl;
//   // 在请求返回后获取当前时间
//   auto end = std::chrono::high_resolution_clock::now();

//   // 计算并打印请求的持续时间
//   auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
//   std::cout << "RPC took " << duration.count() << " nanoseconds.\n";
  return 0;
}