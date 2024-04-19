#include <stdio.h>
#include "rpc.h"

static const std::string kServerHostname = "192.168.0.204";
static const std::string kClientHostname = "192.168.0.203";
// static const std::string kServerHostname = "127.0.0.0";
// static const std::string kClientHostname = "127.0.0.0";
static  uint16_t kUDPPort_s = 31820;
static  uint16_t kUDPPort_c = 31860;
static constexpr uint8_t kReqType = 2;
static constexpr size_t kMsgSize = 16;
