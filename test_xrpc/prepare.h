#pragma once

#include "../src/huibuffer/flat_log.h"
#include "../src/huibuffer/ctrl_buffer.h"
#include "../src/huibuffer/serialization.h"

namespace erpc{


    request get_flat_req(int len);
    response *get_flat_resp(int len);


}
