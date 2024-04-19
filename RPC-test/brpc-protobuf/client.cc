// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// A client sending requests to server every 1 second.
#include <numeric>
#include <fstream>
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include <brpc/channel.h>
#include "flat.pb.h"
#include "util.h"
DEFINE_string(protocol, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, " 192.168.0.204", "IP Address of server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");
// DEFINE_int32(port,50051,"TCP Port of this server");

DEFINE_int32(data_len,1024,"the string length of request");
DEFINE_int32(port,50051,"TCP Port of this server");
DEFINE_uint32(total_request, 1000, "the number of request per processes\n");
DEFINE_string(log_file, "latency_test.txt", "the number of request per processes\n");




test::FlatLogFileFlushChunkRequestPB request;
test::FlatLogFileFlushChunkResponsePB reply;
std::ofstream outfile;
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
int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    gflags::ParseCommandLineFlags(&argc, &argv, true);


    outfile.open(FLAGS_log_file+std::to_string(FLAGS_data_len));
    brpc::Channel channel;
    
    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.connection_type = FLAGS_connection_type;
    options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    options.max_retry = FLAGS_max_retry;
    auto server_address = FLAGS_server + ":" + std::to_string(FLAGS_port);
    if (channel.Init(server_address.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    std::vector<double> latency;
    std::vector<double> latency2;
    latency.reserve(FLAGS_total_request+1);
    test::Test_Stub stub(&channel);
    request = get_flat_req((FLAGS_data_len-352)/3+1);
    printf("send message size is %d\n",request.ByteSize());
    for(uint32_t i = 0 ; i < uint32_t(FLAGS_total_request+1) ; i ++){
        auto start = std::chrono::system_clock::now(); // 开始
        brpc::Controller cntl;
        stub.Echo(&cntl, &request, &reply, NULL);
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<Nanoseconds>(end-start);
        if(i)latency.emplace_back(double(duration.count()));
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
    return 0;
}



/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
  // 请求次数
  uint32_t total_request = 1000;
  // 存储每次请求的时间
  std::vector<double> durations;

  // totaltime for request 总时间
  std::chrono::duration<double, std::micro> totalTime(0);
*/