
#include "prepare.h"

namespace erpc
{
    static std::string str;
    
    ctrl_buffer *get_req()
    {
    }

    ctrl_buffer_array *get_req_array()
    {
    }

    ctrl_buffer *get_resp()
    {
    }

    ctrl_buffer_array *get_resp_array()
    {
    }

    
    static request req;
    static ReplicaLocationPB replicaLocationPB;
    request get_flat_req(int len)
    {
        if(str.size() != 0)
            str = std::string(len, 'h');

        req.Data.assign(str);
        req.HostName.assign("https://192.168.0.201:10889");
        req.UserId.assign("4f062dd5-05e5-48e3-821e-35e46ca028ce");

        req.OpenChunkOptions.ChunkOptions.Placement.assign(str);
        replicaLocationPB.CSLocation.assign(str);
        req.OpenChunkOptions.ChunkLocation.assign(&replicaLocationPB, 1);
        req.Strategy.Strategies._len = 0;

        return req;
    }

    static response *resp;
    response *get_flat_resp(int len)
    {
        if(str.size() != 0)
            str = std::string(len, 'h');
        resp = new response();
        resp->trace_data.assign(str);
        return resp;
    }
}