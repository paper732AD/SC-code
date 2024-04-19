#pragma once

#include <iostream>
#include "../../../serialize.h"


// extern std::string str;
// extern std::vector<RPC::string> str_array;

struct IID_Interface
{
    const static uint32_t IID = 111;
}; // IID: InterfaceID
struct Greeter: public IID_Interface {
    struct SayHelloAll {
        const static uint32_t FID = 0; // FID: FunctionID/MethodID
        struct Request: public RPC::Message{
            RPC::string a;
            PROCESS_FIELDS(a);
        };
        struct Response: public RPC::Message{
            RPC::string a;
            PROCESS_FIELDS(a);
        };
    };
};

struct Greeter_array: public IID_Interface {
    struct SayHelloAll_array {
        const static uint32_t FID = 0; // FID: FunctionID/MethodID
        struct Request_array: public RPC::Message{
            RPC::iovec_array a;
            PROCESS_FIELDS(a);
        };
        struct Response_array: public RPC::Message{
            RPC::iovec_array a;
            PROCESS_FIELDS(a);
        };
    };
};

Greeter::SayHelloAll::Request get_req (int);
Greeter::SayHelloAll::Response* get_resp ();
Greeter_array::SayHelloAll_array::Request_array get_req_array (int, int);
Greeter_array::SayHelloAll_array::Response_array* get_resp_array ();






