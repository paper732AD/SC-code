#include "../hello_world/common.h"
#include "../src/huibuffer/flat_log.h"

#include <stdio.h>
#include <atomic>
#include <gflags/gflags.h>
using namespace erpc;

DEFINE_uint32(data_len, 1024, "the string length in flag_log_resp, 0-10240\n");
DEFINE_uint32(processes, 1, "the number of processes to run client\n");
DEFINE_bool(record_time, false, "the number of request\n");

erpc::Rpc<erpc::CTransport> *rpc;
erpc::Serializer respser[16];
response *resp;
request *req;
pkthdr_t* _hdr;
std::atomic<int> count;

Buffer _vbuf,_vstring,_vhdr;
uint32_t  total_pkg;
std::string data;
int No; 

void init_Buffer (){
    // allocate memory from hugepages
    _vbuf = rpc->alloc_buffer(sizeof(response)*16);
    _vstring = rpc->alloc_buffer(sizeof(char)*1124000);
      
    // resp = new(_vbuf.buf_) response();
    resp = static_cast<response*>((void*)_vbuf.buf_);
    data = std::string(FLAGS_data_len-80,'h');
    sprintf((char*)_vstring.buf_, data.c_str());

    for(int i=0;i<16;++i){
      auto _resp = new(resp+i) response();
      _resp->a = 54321;
      _resp->trace_data.assign((const char*)(_vstring.buf_));
    }

    //try to serialize
    respser[0].serialize(*resp);
    total_pkg = respser[0].init_data_iov(erpc::CTransport::kMaxDataPerPkt);
    _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t)*total_pkg);
    _hdr = new(_vhdr.buf_) pkthdr_t[total_pkg];
    _hdr->total_pkt = total_pkg;
    ERPC_WARN("respser.iov.sum() = %d\n",respser[0].iov.sum());
    ERPC_INFO("init_Buffer successed\n");
}

void req_handler(erpc::ReqHandle *req_handle, void* _t) {
  ERPC_INFO("in req_handler, req_handle->dyn_resp_msgbuf_.iov.sum() = %d\n",req_handle->dyn_resp_msgbuf_.iov.sum());
  // for(int i=0;i<req_handle->dyn_resp_msgbuf_.iov.iovcnt();++i){
  //    std::cout << req_handle->dyn_resp_msgbuf_.iov[i].iov_len << " ";
  // }
  // std::cout << std::endl;
  // ERPC_WARN("iov.iovcnt()=%d, iov.sum()=%d\n",req_handle->dyn_resp_msgbuf_.iov.iovcnt(), req_handle->dyn_resp_msgbuf_.iov.sum());
  req_handle->server_info_.req_msgbuf_ = (void*)req_handle->dyn_deserializer.deserialize<request>(&req_handle->dyn_resp_msgbuf_.iov);
  int index = req_handle->session_->local_session_num_;
  request* req = static_cast<request*>(req_handle->get_req_msgbuf());
  ERPC_INFO("deserialize finished\n");
  ERPC_INFO("number = req->a = %d\n", req->a);
  respser[index].clear();
  (resp+index)->a = req->a;
  respser[index].serialize(*(resp+index));
  ERPC_INFO("respser->sum() = %d\n", respser[index].iov.sum());
  respser[index].init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  
  rpc->enqueue_response(req_handle, &respser[index], uint32_t(_vbuf.lkey_), _hdr);
}

int main(int argc, char**argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if(FLAGS_record_time == false){
    record_time_flag = false;
  }else{
    record_time_flag = true;
  }
  
  No = 0;
  pid_t pid;
  uint32_t sub_process=1; // fork times = 8-1
  while(sub_process++ < FLAGS_processes){
    pid = fork();
    if(pid == 0) {
      kUDPPort_s = kUDPPort_s+sub_process;
      break;
    }
    else if(pid < 0) {
      ERPC_ERROR("fork failed at %u.\n", sub_process);
    }
    No++;
  }

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort_s);
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
