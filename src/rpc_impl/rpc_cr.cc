#include "rpc.h"
#include "util/timer.h"
namespace erpc {


template <class TTr>
void Rpc<TTr>::enqueue_cr_st(SSlot *sslot, const pkthdr_t *req_pkthdr) {
  assert(in_dispatch());
  // TODO request for response, 取 msgbuffer 的 头的操作怎么弄。
  // TODO头：定义一个头，并将其放进msgbuffer里面。

  int index = ctrl_msgbuf_head_++;
  if (ctrl_msgbuf_head_ == 2 * TTr::kUnsigBatch) 
    ctrl_msgbuf_head_ = 0;// 循环

  // Fill in the CR packet header. Avoid copying req_pkthdr's headroom.
  // TODO 理论上这个头需要定义在前面
  ERPC_INFO("In enqueue_cr_st, fill header\n");
  pkthdr_t *cr_pkthdr = ctrl_msgbufs_hdr+index;
  
  cr_pkthdr->req_type_ = req_pkthdr->req_type_;
  cr_pkthdr->msg_size_ = sizeof(erpc::ctrl_buffer);
  cr_pkthdr->dest_session_num_ = sslot->session_->remote_session_num_;
  cr_pkthdr->pkt_type_ = PktType::kExplCR;
  cr_pkthdr->pkt_num_ = req_pkthdr->pkt_num_;
  cr_pkthdr->req_num_ = req_pkthdr->req_num_;
  cr_pkthdr->magic_ = kPktHdrMagic;
  cr_pkthdr->this_msg_size = 0;

  ERPC_INFO("In enqueue_cr_st, header filled\n");

  enqueue_hdr_tx_burst_st(sslot, &(ctrl_serialization[index]), nullptr);
}

template <class TTr>
void Rpc<TTr>::process_expl_cr_st(SSlot *sslot, const pkthdr_t *pkthdr,
                                  size_t rx_tsc) {
  assert(in_dispatch());
  assert(pkthdr->req_num_ <= sslot->cur_req_num_);

  // Handle reordering
  // if (unlikely(!in_order_client(sslot, pkthdr))) {
  //   ERPC_REORDER(
  //       "Rpc %u, lsn %u (%s): Received out-of-order CR. "
  //       "Packet %zu/%zu, sslot: %zu/%s. Dropping.\n",
  //       rpc_id_, sslot->session_->local_session_num_,
  //       sslot->session_->get_remote_hostname().c_str(), pkthdr->req_num_,
  //       pkthdr->pkt_num_, sslot->cur_req_num_, sslot->progress_str().c_str());
  //   return;
  // }

  // Update client tracking metadata
  if (kCcRateComp) update_timely_rate(sslot, pkthdr->pkt_num_, rx_tsc);
  ERPC_INFO("expl_cr call bump_credits()\n");
  ERPC_INFO("pkthdr->pkt_num_ = %d, pkthdr->total_pkt  = %d\n",pkthdr->pkt_num_, sslot->tx_msgbuf_->get_n_hdr(0)->total_pkt);
  // if(pkthdr->pkt_num_ == 0 )
  //   // first_expl_cr[lat_client_count-1]=double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
  //   first_expl_cr.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  // if(pkthdr->pkt_num_ == sslot->tx_msgbuf_->get_n_hdr(0)->total_pkt - 2 )
  //   // last_expl_cr[lat_client_count-1]=double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
  //   last_expl_cr.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  
  bump_credits(sslot->session_);
  // sslot->client_info_.num_rx_++;
  sslot->client_info_.progress_tsc_ = ev_loop_tsc_;

  // If we've transmitted all request pkts, there's nothing more to TX yet
  if (req_pkts_pending(sslot)) {
    ERPC_INFO("req_pkts_pending(sslot) is true\n");
    kick_req_st(sslot);  // credits >= 1
  }
}

FORCE_COMPILE_TRANSPORTS

}  // namespace erpc
