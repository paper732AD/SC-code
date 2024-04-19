// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: greeter
#ifndef GRPC_greeter__INCLUDED
#define GRPC_greeter__INCLUDED

#include "greeter_generated.h"
#include "flatbuffers/grpc.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

class Greeter final {
 public:
  static constexpr char const* service_full_name() {
    return "Greeter";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status SayHello(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>> AsyncSayHello(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>>(AsyncSayHelloRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>> PrepareAsyncSayHello(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>>(PrepareAsyncSayHelloRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* AsyncSayHelloRaw(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* PrepareAsyncSayHelloRaw(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status SayHello(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>> AsyncSayHello(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>>(AsyncSayHelloRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>> PrepareAsyncSayHello(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>>(PrepareAsyncSayHelloRaw(context, request, cq));
    }
  
   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* AsyncSayHelloRaw(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* PrepareAsyncSayHelloRaw(::grpc::ClientContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_SayHello_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
  
  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status SayHello(::grpc::ServerContext* context, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>* request, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_SayHello : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service */*service*/) {}
   public:
    WithAsyncMethod_SayHello() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_SayHello() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status SayHello(::grpc::ServerContext* /*context*/, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>* /*request*/, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* /*response*/) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestSayHello(::grpc::ServerContext* context, flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>* request, ::grpc::ServerAsyncResponseWriter< flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef   WithAsyncMethod_SayHello<  Service   >   AsyncService;
  template <class BaseClass>
  class WithGenericMethod_SayHello : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service */*service*/) {}
   public:
    WithGenericMethod_SayHello() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_SayHello() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status SayHello(::grpc::ServerContext* /*context*/, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>* /*request*/, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* /*response*/) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_SayHello : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service */*service*/) {}
   public:
    WithStreamedUnaryMethod_SayHello() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>(std::bind(&WithStreamedUnaryMethod_SayHello<BaseClass>::StreamedSayHello, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_SayHello() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status SayHello(::grpc::ServerContext* /*context*/, const flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>* /*request*/, flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>* /*response*/) final override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedSayHello(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< flatbuffers::grpc::Message<FlatLogFileFlushChunkRequestFB>,flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB>>* server_unary_streamer) = 0;
  };
  typedef   WithStreamedUnaryMethod_SayHello<  Service   >   StreamedUnaryService;
  typedef   Service   SplitStreamedService;
  typedef   WithStreamedUnaryMethod_SayHello<  Service   >   StreamedService;
};


#endif  // GRPC_greeter__INCLUDED
