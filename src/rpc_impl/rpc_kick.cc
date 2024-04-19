#include "rpc.h"

namespace erpc {

template <class TTr>
void Rpc<TTr>::kick_req_st(SSlot *sslot) {
  assert(in_dispatch());
  auto &credits = sslot->session_->client_info_.credits_;
  assert(credits > 0);  // Precondition

  auto &ci = sslot->client_info_;
  // 每发一个请求令牌-1，收到一个credit/last response 令牌+1
  size_t sending =
      (std::min)(credits, static_cast<pkthdr_t*>(sslot->tx_msgbuf_->iov[0].iov_base)->total_pkt - ci.num_tx_);
  // 不拥塞就直接发 
  ERPC_INFO("sending=%d, credits=%d, sslot->tx_msgbuf_->iov[0].iov_base)->total_pkt=%d, ci.num_tx_=%d\n",
    sending, credits, static_cast<pkthdr_t*>(sslot->tx_msgbuf_->iov[0].iov_base)->total_pkt,ci.num_tx_);
  bool bypass = can_bypass_wheel(sslot);
  ERPC_INFO("enqueue %d packages\n",sending);
  for (size_t x = 0; x < sending; x++) {
    // if (bypass) {
      ERPC_INFO("\tenqueue_pkt_tx_burst_st(sslot, ci.num_tx(pkt_idx)=%d)\n",ci.num_tx_);
      enqueue_pkt_tx_burst_st(sslot, ci.num_tx_ /* pkt_idx */,
                              &ci.tx_ts_[ci.num_tx_ % kSessionCredits]);//时间
    // } else {
      // enqueue_wheel_req_st(sslot, ci.num_tx_);
    // }

    ci.num_tx_++;
    credits--;

  }
}

// We're asked to send RFRs, which means that we have recieved the first
// response packet, but not the entire response. The latter implies that a
// background continuation cannot invalidate resp_msgbuf.
template <class TTr>
void Rpc<TTr>::kick_rfr_st(SSlot *sslot, const pkthdr_t* pkthdr) {
  assert(in_dispatch());
  auto &credits = sslot->session_->client_info_.credits_;
  auto &ci = sslot->client_info_;

  assert(credits > 0);  // Precondition

  // TODO: Pace RFRs
  size_t rfr_pndng = wire_pkts(sslot->tx_msgbuf_->get_n_hdr(0)->total_pkt, pkthdr->total_pkt) - ci.num_tx_;
  size_t sending = (std::min)(credits, rfr_pndng);  // > 0
  // ERPC_WARN("int kick_rfr_st, credits = %d, rfr_pndng = %d, sending = %d, pkthdr->pkg_num_ = %d\n", credits, rfr_pndng, sending, pkthdr->pkt_num_);
  for (size_t x = 0; x < sending; x++) {
    enqueue_rfr_st(sslot, pkthdr);
    ci.num_tx_++;
    credits--;
  }
}

FORCE_COMPILE_TRANSPORTS

}  // namespace erpc
