#include <stdexcept>
#include "huibuffer/serialization.h"
#include "rpc.h"

namespace erpc {

// template <class TTr>
// void enqueue_request(int session_num, uint8_t req_type, Serializer *req_msgbuf,
//                       void* &resp_msgbuf, erpc_cont_func_t cont_func,
//                       void *tag, uint32_t _lkey, pkthdr_t *pkthdr, size_t cont_etid){
//                           ERPC_ERROR("****************************enqueue_request void****************************\n");
//                       }

// The cont_etid parameter is passed only when the event loop processes the
// background threads' queue of enqueue_request calls.
template <class TTr>
void Rpc<TTr>::enqueue_request(int session_num, uint8_t req_type,
                               Serializer *req_msgbuf, void** resp_msgbuf,
                               erpc_cont_func_t cont_func, void *tag,
                                uint32_t _lkey, pkthdr_t* pkthdr, void* context,size_t cont_etid) {
   //When called from a background thread, enqueue to the foreground thread
   if (unlikely(!in_dispatch())) {
     auto req_args = enq_req_args_t(session_num, req_type, req_msgbuf,
                                    resp_msgbuf, cont_func, tag, get_etid());
     bg_queues_.enqueue_request_.unlocked_push(req_args);
     return;
   }

  // If we're here, we're in the dispatch thread
  Session *session = session_vec_[static_cast<size_t>(session_num)];
  assert(session->is_connected());  // User is notified before we disconnect

  // If a free sslot is unavailable, save to session backlog
  if (unlikely(session->client_info_.sslot_free_vec_.size() == 0)) {
    session->client_info_.enq_req_backlog_.emplace(session_num, req_type,
                                                   req_msgbuf, resp_msgbuf,
                                                   cont_func, tag, cont_etid);
    return;
  }

  // Fill in the sslot info
  size_t sslot_i = session->client_info_.sslot_free_vec_.pop_back();
  ERPC_INFO("get a free client_info\n");
  SSlot &sslot = session->sslot_arr_[sslot_i];
  assert(sslot.tx_msgbuf_ == nullptr);  // Previous response was received
  // ! caution !!!!
  sslot.tx_msgbuf_ = req_msgbuf;        // Mark the request as active/incomplete
  sslot.cur_req_num_ += kSessionReqWindow;  // Move to next request
  if(context!=nullptr){
    sslot.c = context;
  }
  auto &ci = sslot.client_info_;
  // * 这里设置了client_info的resp_msgbuf，cont_func，tag，ev_loop_tsc_，num_rx_，num_tx_
  ci.resp_msgbuf_ = resp_msgbuf;
  ci.cont_func_ = cont_func;
  ci.tag_ = tag;
  ci.progress_tsc_ = ev_loop_tsc_;
  add_to_active_rpc_list(sslot);

  ci.num_rx_ = 0;
  ci.num_tx_ = 0;
  ci.cont_etid_ = cont_etid;

  // * 填充零号头的内容
  // Fill in packet 0's header
  pkthdr_t *pkthdr_0 = pkthdr;
  pkthdr_0->req_type_ = req_type;
  pkthdr_0->msg_size_ = req_msgbuf->iov.sum();
  ERPC_INFO("req_msgbuf->iov.sum() = %d\n",req_msgbuf->iov.sum());
  pkthdr_0->dest_session_num_ = session->remote_session_num_;
  pkthdr_0->pkt_type_ = PktType::kReq;
  pkthdr_0->pkt_num_ = 0;
  pkthdr_0->req_num_ = sslot.cur_req_num_;
  pkthdr_0->lkey = _lkey;
  pkthdr_0->magic_ = kPktHdrMagic;
  pkthdr_0->this_msg_size = TTr::kMaxDataPerPkt;
  // 这里要把包头放在前面。

  // * 填充非零号头的内容，我们的RPC不需要这块。
  // * 用零号头填充非零号头的内容。
  ERPC_INFO("Before init other header\n");
  for(int i=1; i<pkthdr->total_pkt; ++i){
    memcpy(pkthdr+i, pkthdr, sizeof(pkthdr_t));
    (pkthdr+i)->pkt_num_ = i;
    ERPC_INFO("(pkthdr+%d)->pkt_num_ = %d\n",i,i);
  }
  ERPC_INFO("Before modify the tail header of the request\n");
  (pkthdr+(pkthdr->total_pkt-1))->this_msg_size = pkthdr->msg_size_%(TTr::kMaxDataPerPkt);
  ERPC_INFO("push headers front of iovector\n");
  for(int i=pkthdr->total_pkt-1;i>=0;--i){
    auto ret = req_msgbuf->iov.push_front(pkthdr+i, sizeof(pkthdr_t));
    if(ret != sizeof(pkthdr_t)){
      ERPC_ERROR("push header to req_msgbuffer error !\n");
    } 
  }
  ERPC_INFO("pushed headers front of iovector\n");

  // Fill in any non-zeroth packet headers, using pkthdr_0 as the base.
  // if (unlikely(req_msgbuf->num_pkts_ > 1)) {
  //   for (size_t i = 1; i < req_msgbuf->num_pkts_; i++) {
  //     pkthdr_t *pkthdr_i = req_msgbuf->get_pkthdr_n(i);//拿到第n个包的位置，
  //     memcpy(pkthdr_i, pkthdr_0, sizeof(pkthdr_t));//然后用第零个包的内容信息填充
  //     pkthdr_i->pkt_num_ = i;//将自己的序号改成i
  //   }
  // }
  //如果credit大于零
  if (likely(session->client_info_.credits_ > 0)) {
    ERPC_INFO("kick_req_st(&sslot);\n");
    kick_req_st(&sslot);
  } else {
    ERPC_INFO("session->client_info_.credits_ <= 0\n");
    stallq_.push_back(&sslot);
  }
}

template <class TTr>
void Rpc<TTr>::process_small_req_st(SSlot *sslot, pkthdr_t *pkthdr, uint8_t *body) {
  ERPC_INFO("process_small_req_one_st\n");
// 这里把包头的信息读出来然后把rxbuffer的内容拷贝出来
  assert(in_dispatch());

  // If we're here, this is the first (and only) packet of this new request
  assert(pkthdr->req_num_ == sslot->cur_req_num_ + kSessionReqWindow);

  // auto req_msgbuf = sslot->server_info_.req_msgbuf_;
  // assert(req_msgbuf.is_buried());  // Buried on prev req's enqueue_response()

  // Bury the previous, possibly dynamic response (sslot->tx_msgbuf). This marks
  // the response for cur_req_num as unavailable.
  // bury_resp_msgbuf_server_st(sslot);

  // Update sslot tracking
  sslot->cur_req_num_ = pkthdr->req_num_;
  sslot->server_info_.num_rx_ = 1;

  const ReqFunc &req_func = req_func_arr_[pkthdr->req_type_];

  // Remember request metadata for enqueue_response(). req_type was invalidated
  // on previous enqueue_response(). Setting it implies that an enqueue_resp()
  // is now pending; this invariant is used to safely reset sessions.
  assert(sslot->server_info_.req_type_ == kInvalidReqType);
  sslot->server_info_.req_type_ = pkthdr->req_type_;
  sslot->server_info_.req_func_type_ = req_func.req_func_type_;

  if (likely(!req_func.is_background())) {
    // *********************** 这里应该反序列化出来
    auto& req_iov = sslot->dyn_resp_msgbuf_.iov;//用来存这个request
    req_iov.iov_begin = req_iov.iov_end = 0;
    req_iov.push_back(body, pkthdr->msg_size_);
    ERPC_INFO("pkthdr->msg_size_ = %d\n",pkthdr->msg_size_);
    ERPC_INFO("req_iov[0]->iov_base = %d\n",req_iov.begin()->iov_base);
    ERPC_INFO("req_iov[0]->iov_base = 0x%X\n",req_iov.begin()->iov_base);
    ERPC_INFO("req_iov[0]->iov_len = %d\n",req_iov.begin()->iov_len);

    // sslot->server_info_.req_msgbuf_ = (void*)ctrl_buffer::deserialize(&req_iov);
    req_func.req_func_(static_cast<ReqHandle *>(sslot), context_);
    return;
  } 
}

template <class TTr>
void Rpc<TTr>::process_large_req_one_st(SSlot *sslot, pkthdr_t *pkthdr, uint8_t *body) {
  ERPC_INFO("process_large_req_one_st\n");

  assert(in_dispatch());
  ERPC_INFO("in_dispatch()\n");

  // Handle reordering
  bool is_next_pkt_same_req =  // Is this the next packet in this request? 还是这个请求的包
      (pkthdr->req_num_ == sslot->cur_req_num_) &&
      (pkthdr->pkt_num_ == sslot->server_info_.num_rx_);
  bool is_first_pkt_next_req =  // Is this the first packet in the next request? 下个请求的第一个包
      (pkthdr->req_num_ == sslot->cur_req_num_ + kSessionReqWindow) &&
      (pkthdr->pkt_num_ == 0);


  auto& req_msgbuf = sslot->dyn_resp_msgbuf_;//用来存这个request
  // Allocate or locate the request MsgBuffer
  if (pkthdr->pkt_num_ == 0) {

    req_msgbuf.iov.iov_begin = req_msgbuf.iov.iov_end = 0;
      ERPC_INFO("REQ: New one\n");

    // Update sslot tracking
    sslot->cur_req_num_ = pkthdr->req_num_;
    sslot->server_info_.num_rx_ = 1;
  } else {
    sslot->server_info_.num_rx_++;
  }

  // !如果客戶端3個包。服務端發一個包，就會有問題。
  if (pkthdr->pkt_num_ != pkthdr->total_pkt - 1) {
    ERPC_INFO("enqueue_cr_st(sslot, pkthdr); pkthdr->pkt_num_= %d, pkthdr->total_pkt = %d\n",pkthdr->pkt_num_ ,pkthdr->total_pkt);
    enqueue_cr_st(sslot, pkthdr);//给出了最后一个包的每一个包都返回一个credit，用来控制流量
  }
  // ERPC_INFO();
  if((static_cast<uint8_t*>(req_msgbuf.iov.back().iov_base)+req_msgbuf.iov.back().iov_len) == body){
    ERPC_INFO("REQ: happy life !!! data is sequential!!!!, index is %d\n", pkthdr->pkt_num_);
    req_msgbuf.iov.back().iov_len += pkthdr->this_msg_size;
  }else{
    ERPC_INFO("REQ: Oh no, why it is not sequential?\n");
    auto ret = req_msgbuf.iov.push_back(body, pkthdr->this_msg_size);
    if(ret != pkthdr->this_msg_size){
      ERPC_ERROR("ERRPR: auto ret = req_msgbuf.iov.push_back(body, pkthdr->this_msg_size);\n");
      return ;
    }
  }
  ERPC_INFO("push_back successful; pkthdr->this_msg_size = %d\n",pkthdr->this_msg_size);
  ERPC_INFO("sslot->server_info_.num_rx_ = %d, pkthdr->total_pkt = %d\n", sslot->server_info_.num_rx_, pkthdr->total_pkt);

//   // Invoke the request handler iff we have all the request packets
  if (sslot->server_info_.num_rx_ != pkthdr->total_pkt) 
      return;
  // ERPC_WARN("req_msgbuf.iov.sum() = %u, req_msgbuf.iov.iovcnt() = %d\n", req_msgbuf.iov.sum(), req_msgbuf.iov.iovcnt());

  // auto __req_msgbuffer = ctrl_buffer::deserialize(&req_msgbuf.iov);

  const ReqFunc &req_func = req_func_arr_[pkthdr->req_type_];

  // Remember request metadata for enqueue_response(). req_type was invalidated
  // on previous enqueue_response(). Setting it implies that an enqueue_resp()
  // is now pending; this invariant is used to safely reset sessions.
  assert(sslot->server_info_.req_type_ == kInvalidReqType);
  sslot->server_info_.req_type_ = pkthdr->req_type_;
  sslot->server_info_.req_func_type_ = req_func.req_func_type_;
  // sslot->server_info_.req_msgbuf_ = (void*)__req_msgbuffer;
  
//   // req_msgbuf here is independent of the RX ring, so don't make another copy
  if (likely(!req_func.is_background())) {
    req_func.req_func_(static_cast<ReqHandle *>(sslot), context_);
  } else {
    submit_bg_req_st(sslot);
  }
}

FORCE_COMPILE_TRANSPORTS

}  // namespace erpc
