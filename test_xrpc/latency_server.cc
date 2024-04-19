#include "../hello_world/common.h"
#include "stdio.h"
#include <atomic>
#include <gflags/gflags.h>
using namespace erpc;

DEFINE_uint32(data_len, 1024, "the string length in ctrl_bufer, 0-10240\n");
DEFINE_bool(record_time, false, "the number of request\n");
DEFINE_uint32(server_port, 31860, "the string length in ctrl_bufer, 0-102410\n");

erpc::Rpc<erpc::CTransport> *rpc;
erpc::Serializer respser;
ctrl_buffer *resp, *req;
pkthdr_t* _hdr;
std::atomic<int> count;

Buffer _vbuf,_vstring,_vhdr;
uint32_t  total_pkg;
std::string data;
int one, multi;

void init_Buffer (){
    // allocate memory from hugepages
    _vbuf = rpc->alloc_buffer(sizeof(ctrl_buffer));
    _vstring = rpc->alloc_buffer(sizeof(char)*1124000);
    ERPC_INFO("alloc_buffer successed\n");
      
    resp = new(_vbuf.buf_) ctrl_buffer();
    ERPC_INFO("new successed\n");
    data = std::string(FLAGS_data_len,'h');
    resp->a = 54321;
    ERPC_INFO("resp->a successed\n");
    //TODO maybe change the length
    sprintf((char*)_vstring.buf_, data.c_str());
    ERPC_INFO("sprintf successed\n");
    resp->hahaha.assign((const char*)(_vstring.buf_));
    ERPC_INFO("assign successed\n");
    respser.serialize(*resp);
    total_pkg = respser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
    _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t)*total_pkg);
    _hdr = new(_vhdr.buf_) pkthdr_t[total_pkg];
    _hdr->total_pkt = total_pkg;
    ERPC_INFO("init_Buffer successed\n");
    ERPC_WARN("the size of message sent is %d\n", respser.iov.sum());
    one=0;
    multi=0;
}

void req_handler(erpc::ReqHandle *req_handle, void* _t) {
  // ERPC_WARN("iov.iovcnt()=%d, iov.sum()=%d\n",req_handle->dyn_resp_msgbuf_.iov.iovcnt(), req_handle->dyn_resp_msgbuf_.iov.sum());
  //  if(req_handle->dyn_resp_msgbuf_.iov.iovcnt() == 1){
  //   one++;
  // }else{
  //   multi++;
  // }
  req_handle->server_info_.req_msgbuf_ = (void*)ctrl_buffer::deserialize(&req_handle->dyn_resp_msgbuf_.iov);
  ctrl_buffer* req = static_cast<ctrl_buffer*>(req_handle->get_req_msgbuf());
  ERPC_INFO("number = req->a = %d\n", req->a);
  respser.clear();
  resp->a = req->a;
  respser.serialize(*resp);
  respser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  // TODO 这里就只是放普通的请求回复
  rpc->enqueue_response(req_handle, &respser, uint32_t(_vbuf.lkey_), _hdr);

  // ERPC_WARN("count = %d one = %d, multi = %d\n",++count, one, multi);
}

int main(int argc, char**argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if(FLAGS_record_time == false){
    record_time_flag = false;
  }else{
    record_time_flag = true;
  }
  
  std::string server_uri = kServerHostname + ":" + std::to_string(FLAGS_server_port);
  erpc::Nexus nexus(server_uri);
  nexus.register_req_func(kReqType, req_handler);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  ERPC_WARN("sizeof(pkthdr_t)=%d\n",sizeof(pkthdr_t));
  
  init_Buffer();

  rpc->run_event_loop(10000000);

  rpc->free_msg_buffer(_vbuf);
  rpc->free_msg_buffer(_vhdr);
  rpc->free_msg_buffer(_vstring);

  delete rpc;
}
