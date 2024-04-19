#pragma once

#include <vector>
#include <map>

// #include "serialization.h"
#include "ctrl_buffer.h"


namespace erpc{
class huiBuffer {
private:
    typedef void*(*BUFFER_FUNC)();

public:
    enum BUF_TYPE {
        CTRL_BUF,
    };
    // 用来存放 req 构造函数的 函数指针
    std::vector<BUFFER_FUNC> req_func_vec;
    std::vector<BUFFER_FUNC> resp_func_vec;

public:
    huiBuffer (){
        // Index zero is for the ctrl message,
        // which contains no message in data field. 
        // all information is maintained in header partation.
        // control package include: connection/disconnection/credict/requestforresponse
        req_func_vec.emplace_back(ctrl_buffer::create);
        resp_func_vec.emplace_back(ctrl_buffer::create);

    }

    ctrl_buffer get_ctrl_buffer (){
        return ctrl_buffer();
    }

    void requset_register (BUFFER_FUNC func, uint32_t req_type){
        assert(req_type > 0);// cannot less than 0
        assert(req_type < 1000000);// cannot larger than 1000000
        assert(req_func_vec[req_type] == nullptr);
            
        if(req_type >= req_func_vec.size())
            req_func_vec.resize(req_type+1);
        req_func_vec[req_type] = func;
    }

    void response_register (BUFFER_FUNC func, uint32_t resp_type){
        assert(resp_type > 0);
        assert(resp_type < 1000000);        
        assert(resp_func_vec[resp_type] == nullptr);

        if(resp_type >= resp_func_vec.size())
            resp_func_vec.resize(resp_type+1);
        resp_func_vec[resp_type] = func;
    }

    void* get_request (uint32_t req_type){
        return req_func_vec[req_type]();
    }

    void* get_response (uint32_t resp_type){
        return resp_func_vec[resp_type]();
    }
    
};
}