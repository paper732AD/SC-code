#pragma once
#include <iostream>
#include "../../../serialize.h"

struct leaf: public RPC::Message{
    RPC::string a;
    // template<typename AR>
    // void serialize_fields(AR& ar){
    //     ar.process_field(a);
    // }
        PROCESS_FIELDS(a);

};

struct Tree1 : public RPC::Message{
    leaf Left;
    leaf Right;
    PROCESS_FIELDS(Left,Right);
};

struct Tree2 : public RPC::Message{
    Tree1 Left;
    Tree1 Right;
    PROCESS_FIELDS(Left,Right);
};