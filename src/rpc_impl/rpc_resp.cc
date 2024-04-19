#include "rpc.h"
#include "huibuffer/serialization.h"
#include "huibuffer/iovector.h"
#include "util/timer.h"
namespace erpc {

// For both foreground and background request handlers, enqueue_response() may
// be called before or after the request handler returns to the event loop, at
// which point the event loop buries the request MsgBuffer.
//
// So sslot->rx_msgbuf may or may not be valid at this point.
template <class TTr>
void Rpc<TTr>::enqueue_response(ReqHandle *req_handle, Serializer *resp_msgbuf, uint32_t _lkey, pkthdr_t* pkthdr, void* context) {
  // When called from a background thread, enqueue to the foreground thread
  if (unlikely(!in_dispatch())) {
    bg_queues_.enqueue_response_.unlocked_push(
        enq_resp_args_t(req_handle, resp_msgbuf));
    return;
  }

  // If we're here, we're in the dispatch thread
  SSlot *sslot = static_cast<SSlot *>(req_handle);
  // sslot->server_info_.sav_num_req_pkts_ =
  //     sslot->server_info_.req_msgbuf_.num_pkts_;
  ERPC_INFO("before sslot->server_info_.sav_num_req_pkts_ = 1\n");
  if(context!=nullptr){
    sslot->c = context;
  }
  sslot->server_info_.sav_num_req_pkts_ = 1;
  ERPC_INFO("before bury_req_msgbuf_server_st(sslot);\n");
  bury_req_msgbuf_server_st(sslot);  // Bury the possibly-dynamic req MsgBuffer
  
  ERPC_INFO("before Session *session = sslot->session_\n");
  Session *session = sslot->session_;
  if (unlikely(!session->is_connected())) {
    // A session reset could be waiting for this enqueue_response()
    assert(session->state_ == SessionState::kResetInProgress);

    ERPC_WARN("Rpc %u, lsn %u: enqueue_response() while reset in progress.\n",
              rpc_id_, session->local_session_num_);

    // Mark enqueue_response() as completed
    assert(sslot->server_info_.req_type_ != kInvalidReqType);
    sslot->server_info_.req_type_ = kInvalidReqType;

    return;  // During session reset, don't add packets to TX burst
  }

  // * 填充零号头的内容。 这个头是有问题的，不能在这里申请，要在enqueue之前申请。
  // Fill in packet 0's header
  ERPC_INFO("Fill in packet 0's header\n");
  pkthdr_t *resp_pkthdr_0 =  pkthdr; //resp_msgbuf->get_pkthdr_0();
  resp_pkthdr_0->req_type_ = sslot->server_info_.req_type_;
  resp_pkthdr_0->msg_size_ = resp_msgbuf->iov.sum();
  resp_pkthdr_0->dest_session_num_ = session->remote_session_num_;
  resp_pkthdr_0->pkt_type_ = PktType::kResp;
  resp_pkthdr_0->pkt_num_ = sslot->server_info_.sav_num_req_pkts_ - 1;
  resp_pkthdr_0->req_num_ = sslot->cur_req_num_;

  resp_pkthdr_0->magic_ = kPktHdrMagic;
  resp_pkthdr_0->lkey = _lkey;
  resp_pkthdr_0->this_msg_size = TTr::kMaxDataPerPkt;

  ERPC_INFO("resp_msgbuf->iov.push_front(resp_pkthdr_0,sizeof(pkthdr_t));\n");
  for(int i = 1; i<pkthdr->total_pkt; ++i){
    memcpy(pkthdr+i, pkthdr, sizeof(pkthdr_t));
    static_cast<pkthdr_t*>(pkthdr+i)->pkt_num_ = i;
  }
  ERPC_INFO("duplicate headers success, total_pkt = %d\n", pkthdr->total_pkt);
  (pkthdr+(pkthdr->total_pkt-1))->this_msg_size = pkthdr->msg_size_%(TTr::kMaxDataPerPkt);
  ERPC_INFO("Set the this msg_size of the tail header.\n");

  for(int i=pkthdr->total_pkt-1;i>=0;--i){
    int ret = resp_msgbuf->iov.push_front(pkthdr+i, sizeof(pkthdr_t));
    if(ret != sizeof(pkthdr_t)){
      ERPC_ERROR("push header to resp_msgbuffer error !\n");
    } 
  }


  ERPC_INFO("it is %d iovec in resp_msgbuf\n",resp_msgbuf->iov.iovcnt());
  ERPC_INFO("resp_msgbuf.iov.sum() = %d \n",pkthdr->msg_size_);

  // Fill in the slot and reset queueing progress
  assert(sslot->tx_msgbuf_ == nullptr);  // Buried before calling request handler
  sslot->tx_msgbuf_ = resp_msgbuf;  // Mark response as valid

  // Mark enqueue_response() as completed
  assert(sslot->server_info_.req_type_ != kInvalidReqType);
  sslot->server_info_.req_type_ = kInvalidReqType;

  //TODO 应该是先发第一个包，然后一个rfr发一个
  // for(int i=0;i<pkthdr->total_pkt;++i){
  //     enqueue_pkt_tx_burst_st(sslot, i, nullptr);  // 0 = packet index, not pkt_num
  // }
  enqueue_pkt_tx_burst_st(sslot, 0, nullptr);  // 0 = packet index, not pkt_num

}

// 处理收到的回复消息
template <class TTr>
void Rpc<TTr>::process_resp_one_st(SSlot *sslot, const pkthdr_t *pkthdr,
                                   size_t rx_tsc, uint8_t* body) {
  ERPC_INFO("In process_resp_one_st()\n");
  assert(in_dispatch());
  assert(pkthdr->req_num_ <= sslot->cur_req_num_);
  
  ERPC_INFO("pkthdr->index = %d\n", pkthdr->pkt_num_);

  // Handle reordering
  // if (unlikely(!in_order_client(sslot, pkthdr))) {
  //   ERPC_REORDER(
  //       "Rpc %u, lsn %u (%s): Received out-of-order response. "
  //       "Packet %zu/%zu, sslot %zu/%s. Dropping.\n",
  //       rpc_id_, sslot->session_->local_session_num_,
  //       sslot->session_->get_remote_hostname().c_str(), pkthdr->req_num_,
  //       pkthdr->pkt_num_, sslot->cur_req_num_, sslot->progress_str().c_str());
  //   ERPC_WARN("Out-of-order\n");

  //   return;
  // }

  auto &ci = sslot->client_info_;
  // void *resp_msgbuf = ci.resp_msgbuf_;
  auto& resp_msgbuf = sslot->dyn_resp_msgbuf_;
  // Update client tracking metadata
  if (kCcRateComp) 
    update_timely_rate(sslot, pkthdr->pkt_num_, rx_tsc);
  bump_credits(sslot->session_);
  ci.num_rx_++;
  ci.progress_tsc_ = ev_loop_tsc_;

  if (likely(pkthdr->msg_size_ <= TTr::kMaxDataPerPkt)) {
    resp_msgbuf.iov.iov_begin = resp_msgbuf.iov.iov_end = 0;
    resp_msgbuf.iov.push_back(body, pkthdr->this_msg_size);
    ERPC_INFO("in small package pkthdr->this_msg_size=%d\n", pkthdr->this_msg_size);
  } 
  else {
    ERPC_INFO("Part of a big package\n");
    Serializer *&req_msgbuf = sslot->tx_msgbuf_;

    ERPC_INFO("ci.num_tx_ = %d, wire_pkts = %d\n", ci.num_tx_, wire_pkts(req_msgbuf->get_n_hdr(0)->total_pkt, pkthdr->total_pkt));
    if (ci.num_tx_ != wire_pkts(req_msgbuf->get_n_hdr(0)->total_pkt, pkthdr->total_pkt)) {
      // sslot->session_->client_info_.credits_--;
      // sslot->client_info_.num_tx_++;
      kick_rfr_st(sslot, pkthdr);
    }

    if(pkthdr->pkt_num_ == 0){
      ERPC_INFO("RESP: New one\n");
      resp_msgbuf.clear();
      resp_msgbuf.iov.iov_begin = resp_msgbuf.iov.iov_end = 0;
      // // first_receive[lat_client_count-1]=double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
      if(record_time_flag) first_receive.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
    }
    if(resp_msgbuf.iov.iov_begin == 0 && resp_msgbuf.iov.iov_end == 0)
    {
        ERPC_INFO("First packet is %d,we need to create a sg\n", pkthdr->pkt_num_);
        resp_msgbuf.new_buff = new uint8_t[pkthdr->msg_size_];// 申请新的内存
        resp_msgbuf.iov.push_back(body, pkthdr->this_msg_size);
        resp_msgbuf.used_size = pkthdr->this_msg_size;
        memcpy(resp_msgbuf.new_buff,resp_msgbuf.iov.begin()->iov_base,resp_msgbuf.used_size);
        resp_msgbuf.iov.begin()->iov_base = reinterpret_cast<void *>(resp_msgbuf.new_buff);
        resp_msgbuf.iov.begin()->iov_len = pkthdr->msg_size_;
    }
    else{
        ERPC_INFO("%d wuhu! justttt test performance\n", pkthdr->pkt_num_);
        memcpy(resp_msgbuf.new_buff + resp_msgbuf.used_size,body,pkthdr->this_msg_size);
        resp_msgbuf.used_size += pkthdr->this_msg_size;
    }
   

    // TODO Transmit remaining RFRs before response memcpy. We have credits.
    ERPC_INFO("ci.num_rx_=%d;  pkthdr->total_pkt=%d\n",ci.num_rx_, pkthdr->total_pkt);
    if (ci.num_rx_ != pkthdr->total_pkt) {
      ERPC_INFO("return\n");
      return;
    }
      
  }
  ci.num_rx_=0;
  // at_receive[lat_client_count-1]=double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
  if(record_time_flag) at_receive.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  // ERPC_WARN("resp_msgbuf.iov.iovcnt()=%d, resp_msgbuf.iov.sum()=%d\n",resp_msgbuf.iov.iovcnt(), resp_msgbuf.iov.sum());
  *ci.resp_msgbuf_ = &resp_msgbuf;
  // ERPC_INFO("(*ci.resp_msgbuf_)->a = %s\n",(static_cast<ctrl_buffer_array*>(*ci.resp_msgbuf_))->a);
  // after_deserialize[lat_client_count-1]=double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
  // after_deserialize.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  

  // Here, the complete response has been received. All references to sslot must
  // have been removed previously, before invalidating the sslot (done next).
  // 1. The TX batch or DMA queue cannot contain a reference because we drain
  //    it after retransmission.
  // 2. The wheel cannot contain a reference because we (a) wait for sslot to
  //    drain from the wheel before retransmitting, and (b) discard spurious
  //    corresponding packets received for packets in the wheel.
  assert(ci.wheel_count_ == 0);

  sslot->tx_msgbuf_ = nullptr;  // Mark response as received
  delete_from_active_rpc_list(*sslot);
  // Free-up this sslot by copying-out needed fields. The sslot may get re-used
  // immediately if there are backlogged requests, or much later from a request
  // enqueued by a background thread.
  const erpc_cont_func_t cont_func = ci.cont_func_;
  void *tag = ci.tag_;
  const size_t cont_etid = ci.cont_etid_;

  Session *session = sslot->session_;
  session->client_info_.sslot_free_vec_.push_back(sslot->index_);

  // Clear up one request from the backlog if needed
  if (!session->client_info_.enq_req_backlog_.empty()) {
    // We just got a new sslot, and we should have no more if there's backlog
    assert(session->client_info_.sslot_free_vec_.size() == 1);
    enq_req_args_t &args = session->client_info_.enq_req_backlog_.front();
    enqueue_request(args.session_num_, args.req_type_, args.req_msgbuf_,
                    &args.resp_msgbuf_, args.cont_func_, args.tag_,
                    args.cont_etid_);
    session->client_info_.enq_req_backlog_.pop();
  }
  ERPC_INFO("Before likely(cont_etid == kInvalidBgETid)\n");
  if (likely(cont_etid == kInvalidBgETid)) {
    cont_func((void*)sslot, tag);
  } else {
    submit_bg_resp_st(cont_func, tag, cont_etid);
  }
  return;
}

FORCE_COMPILE_TRANSPORTS

}  // namespace erpc
