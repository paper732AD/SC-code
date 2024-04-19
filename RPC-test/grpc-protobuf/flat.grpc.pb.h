// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: flat.proto
#ifndef GRPC_flat_2eproto__INCLUDED
#define GRPC_flat_2eproto__INCLUDED

#include "flat.pb.h"

#include <functional>
#include <grpcpp/generic/async_generic_service.h>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/proto_utils.h>
#include <grpcpp/impl/rpc_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/stub_options.h>
#include <grpcpp/support/sync_stream.h>

namespace test {

class Test final {
 public:
  static constexpr char const* service_full_name() {
    return "test.Test";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status Echo(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::test::FlatLogFileFlushChunkResponsePB* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::test::FlatLogFileFlushChunkResponsePB>> AsyncEcho(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::test::FlatLogFileFlushChunkResponsePB>>(AsyncEchoRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::test::FlatLogFileFlushChunkResponsePB>> PrepareAsyncEcho(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::test::FlatLogFileFlushChunkResponsePB>>(PrepareAsyncEchoRaw(context, request, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void Echo(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB* request, ::test::FlatLogFileFlushChunkResponsePB* response, std::function<void(::grpc::Status)>) = 0;
      virtual void Echo(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB* request, ::test::FlatLogFileFlushChunkResponsePB* response, ::grpc::ClientUnaryReactor* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::test::FlatLogFileFlushChunkResponsePB>* AsyncEchoRaw(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::test::FlatLogFileFlushChunkResponsePB>* PrepareAsyncEchoRaw(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    ::grpc::Status Echo(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::test::FlatLogFileFlushChunkResponsePB* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::test::FlatLogFileFlushChunkResponsePB>> AsyncEcho(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::test::FlatLogFileFlushChunkResponsePB>>(AsyncEchoRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::test::FlatLogFileFlushChunkResponsePB>> PrepareAsyncEcho(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::test::FlatLogFileFlushChunkResponsePB>>(PrepareAsyncEchoRaw(context, request, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void Echo(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB* request, ::test::FlatLogFileFlushChunkResponsePB* response, std::function<void(::grpc::Status)>) override;
      void Echo(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB* request, ::test::FlatLogFileFlushChunkResponsePB* response, ::grpc::ClientUnaryReactor* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::test::FlatLogFileFlushChunkResponsePB>* AsyncEchoRaw(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::test::FlatLogFileFlushChunkResponsePB>* PrepareAsyncEchoRaw(::grpc::ClientContext* context, const ::test::FlatLogFileFlushChunkRequestPB& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Echo_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Echo(::grpc::ServerContext* context, const ::test::FlatLogFileFlushChunkRequestPB* request, ::test::FlatLogFileFlushChunkResponsePB* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_Echo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Echo() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Echo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Echo(::grpc::ServerContext* /*context*/, const ::test::FlatLogFileFlushChunkRequestPB* /*request*/, ::test::FlatLogFileFlushChunkResponsePB* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestEcho(::grpc::ServerContext* context, ::test::FlatLogFileFlushChunkRequestPB* request, ::grpc::ServerAsyncResponseWriter< ::test::FlatLogFileFlushChunkResponsePB>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Echo<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_Echo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Echo() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::test::FlatLogFileFlushChunkRequestPB, ::test::FlatLogFileFlushChunkResponsePB>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::test::FlatLogFileFlushChunkRequestPB* request, ::test::FlatLogFileFlushChunkResponsePB* response) { return this->Echo(context, request, response); }));}
    void SetMessageAllocatorFor_Echo(
        ::grpc::MessageAllocator< ::test::FlatLogFileFlushChunkRequestPB, ::test::FlatLogFileFlushChunkResponsePB>* allocator) {
      ::grpc::internal::MethodHandler* const handler = ::grpc::Service::GetHandler(0);
      static_cast<::grpc::internal::CallbackUnaryHandler< ::test::FlatLogFileFlushChunkRequestPB, ::test::FlatLogFileFlushChunkResponsePB>*>(handler)
              ->SetMessageAllocator(allocator);
    }
    ~WithCallbackMethod_Echo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Echo(::grpc::ServerContext* /*context*/, const ::test::FlatLogFileFlushChunkRequestPB* /*request*/, ::test::FlatLogFileFlushChunkResponsePB* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Echo(
      ::grpc::CallbackServerContext* /*context*/, const ::test::FlatLogFileFlushChunkRequestPB* /*request*/, ::test::FlatLogFileFlushChunkResponsePB* /*response*/)  { return nullptr; }
  };
  typedef WithCallbackMethod_Echo<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Echo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Echo() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Echo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Echo(::grpc::ServerContext* /*context*/, const ::test::FlatLogFileFlushChunkRequestPB* /*request*/, ::test::FlatLogFileFlushChunkResponsePB* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Echo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Echo() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Echo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Echo(::grpc::ServerContext* /*context*/, const ::test::FlatLogFileFlushChunkRequestPB* /*request*/, ::test::FlatLogFileFlushChunkResponsePB* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestEcho(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Echo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Echo() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response) { return this->Echo(context, request, response); }));
    }
    ~WithRawCallbackMethod_Echo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Echo(::grpc::ServerContext* /*context*/, const ::test::FlatLogFileFlushChunkRequestPB* /*request*/, ::test::FlatLogFileFlushChunkResponsePB* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerUnaryReactor* Echo(
      ::grpc::CallbackServerContext* /*context*/, const ::grpc::ByteBuffer* /*request*/, ::grpc::ByteBuffer* /*response*/)  { return nullptr; }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_Echo : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithStreamedUnaryMethod_Echo() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler<
          ::test::FlatLogFileFlushChunkRequestPB, ::test::FlatLogFileFlushChunkResponsePB>(
            [this](::grpc::ServerContext* context,
                   ::grpc::ServerUnaryStreamer<
                     ::test::FlatLogFileFlushChunkRequestPB, ::test::FlatLogFileFlushChunkResponsePB>* streamer) {
                       return this->StreamedEcho(context,
                         streamer);
                  }));
    }
    ~WithStreamedUnaryMethod_Echo() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status Echo(::grpc::ServerContext* /*context*/, const ::test::FlatLogFileFlushChunkRequestPB* /*request*/, ::test::FlatLogFileFlushChunkResponsePB* /*response*/) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedEcho(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::test::FlatLogFileFlushChunkRequestPB,::test::FlatLogFileFlushChunkResponsePB>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_Echo<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_Echo<Service > StreamedService;
};
// brpc服务

}  // namespace test


#endif  // GRPC_flat_2eproto__INCLUDED
