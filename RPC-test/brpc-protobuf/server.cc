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

// A server to receive EchoRequest and send back EchoResponse.

#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/server.h>
#include "flat.pb.h"
#include "util.h"
DEFINE_bool(echo_attachment, true, "Echo attachment as well");
DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_string(listen_addr, "192.168.0.204", "Server listen address, may be IPV4/IPV6/UDS."
            " If this is set, the flag port will be ignored");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(data_len,1024,"the string length of response");
test::FlatLogFileFlushChunkResponsePB rep;
namespace test {
class TestServiceImpl : public Test {
public:
    TestServiceImpl() {}
    virtual ~TestServiceImpl() {}
    virtual void Echo(google::protobuf::RpcController* cntl_base,
                      const FlatLogFileFlushChunkRequestPB* request,
                      FlatLogFileFlushChunkResponsePB* response,
        brpc::ClosureGuard done_guard(done);

        brpc::Controller* cntl =
            static_cast<brpc::Controller*>(cntl_base);
        *response = rep;
    }
};
}  // namespace test

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // Generally you only need one Server.
    brpc::Server server;

    // Instance of your service.
    std::cout << FLAGS_data_len << std::endl;
    test::TestServiceImpl test_service_impl;
    rep = get_flat_resp(FLAGS_data_len-52);
    printf("the size of message is %d\n",(int)rep.ByteSize());
    // Add the service into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use brpc::SERVER_OWNS_SERVICE.
    if (server.AddService(&test_service_impl, 
                          brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    butil::EndPoint point;
    auto server_addr = FLAGS_listen_addr + ":" + std::to_string(FLAGS_port);
    if (!FLAGS_listen_addr.empty()) {
        if (butil::str2endpoint(server_addr.c_str(), &point) < 0) {
            LOG(ERROR) << "Invalid listen address:" << FLAGS_listen_addr;
            return -1;
        }
    } else {
        point = butil::EndPoint(butil::IP_ANY, FLAGS_port);
    }
    // Start the server.
    // point = butil::EndPoint(FLAGS_listen_addr,FLAGS_port);
    brpc::ServerOptions options;
    options.idle_timeout_sec = FLAGS_idle_timeout_s;
    if (server.Start(point, &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
