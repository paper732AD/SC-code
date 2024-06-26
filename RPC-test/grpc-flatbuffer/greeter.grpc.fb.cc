// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: greeter

#include "greeter_generated.h"
#include "greeter.grpc.fb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>

static const char* Greeter_method_names[] = {
  "/Greeter/SayHello",
};

std::unique_ptr< Greeter::Stub> Greeter::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& /*options*/) {
  std::unique_ptr< Greeter::Stub> stub(new Greeter::Stub(channel));
  return stub;
}

Greeter::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel)  , rpcmethod_SayHello_(Greeter_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}
  
::grpc::Status Greeter::Stub::SayHello(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SayHello_, context, request, response);
}

::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* Greeter::Stub::AsyncSayHelloRaw(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>::Create(channel_.get(), cq, rpcmethod_SayHello_, context, request, true);
}

::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* Greeter::Stub::PrepareAsyncSayHelloRaw(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>::Create(channel_.get(), cq, rpcmethod_SayHello_, context, request, false);
}

Greeter::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>(
          std::mem_fn(&Greeter::Service::SayHello), this)));
}

Greeter::Service::~Service() {
}

::grpc::Status Greeter::Service::SayHello(::grpc::ServerContext* /*context*/, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>* /*request*/, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* /*response*/) {
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


