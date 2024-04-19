#include "greeter.grpc.fb.h"
#include "greeter_generated.h"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <gflags/gflags.h>

DEFINE_int32(data_len,1024,"the string length of request");

static std::string temp;
class GreeterServiceImpl final : public Greeter::Service {
  virtual grpc::Status SayHello(
      grpc::ServerContext *context,
      const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB> *request_msg,
      flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB> *response_msg) override {
    flatbuffers::grpc::MessageBuilder builder;

    // We call GetRoot to "parse" the message. Verification is already
    // performed by default. See the notes below for more details.

    // 得到消息
    const FlatLogFileFlushChunkRequestFB *request = request_msg->GetRoot();
    std::cout << request->data() << std::endl;
    // Fields are retrieved as usual with FlatBuffers
    // const std::string &name = request->message()->str();

    // `flatbuffers::grpc::MessageBuilder` is a `FlatBufferBuilder` with a
    // special allocator for efficient gRPC buffer transfer, but otherwise
    // usage is the same as usual.


    temp = std::string(FLAGS_data_len, 'h');

    // const unsigned char* ucharArray = reinterpret_cast<const unsigned char*>(temp.c_str());
    // auto flat_temp = builder.CreateVector(ucharArray, FLAGS_data_len);
    auto flat_temp = builder.CreateString(temp);

    FlatLogFileFlushChunkResponseFBBuilder response_builder(builder);
    // 涉及到内存对齐,对应类型的字段一起add比较好
    response_builder.add_a(123123);
    response_builder.add_errorcode(123123);
    response_builder.add_chunk_status(123123);
    response_builder.add_hint(123123);
    response_builder.add_ack_length(12313);
    response_builder.add_trace_type(123123);
    response_builder.add_storage_id(123123);
    response_builder.add_disk_id(123123);
    // int和uint结束
    response_builder.add_trace_data(flat_temp);
    response_builder.add_handle(123123);
    response_builder.add_request_id(123123);
    response_builder.add_checksum(123123);
    response_builder.add_left_replica_size(123123);

    auto response = response_builder.Finish();
    builder.Finish(response);

    // The `ReleaseMessage<T>()` function detaches the message from the
    // builder, so we can transfer the resopnse to gRPC while simultaneously
    // detaching that memory buffer from the builer.
    *response_msg = mb_.ReleaseMessage<FlatLogFileFlushChunkResponseFB>();
    assert(response_msg->Verify());

    // Return an OK status.
    return grpc::Status::OK;
  }


  flatbuffers::grpc::MessageBuilder mb_;
};

void RunServer() {
  std::string server_address("192.168.0.204:50051");
  GreeterServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cerr << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, const char *argv[]) {
  RunServer();
  return 0;
}
