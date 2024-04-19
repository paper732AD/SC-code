#pragma once
#include "serialization.h"
#include <iostream>
#include "util/logger.h"
#include "util/timer.h"
#include "iovector.h"

namespace erpc{
    struct ctrl_buffer : public erpc::Message
    {
        int a;
        // int a; // for package count
        erpc::string hahaha;
        ctrl_buffer(){
        }
        ~ctrl_buffer(){
        }
        static void* create(){
            return static_cast<void* >(new ctrl_buffer());
        }
        void destory(){
            delete this;
        }
        static ctrl_buffer* deserialize (struct iovector* vec){
            Deserializer deser;
            return deser.deserialize<ctrl_buffer>(vec);
        }
        PROCESS_FIELDS(hahaha);
    };

    struct ctrl_buffer_array : public erpc::Message
    {
        int a; // for package count
        erpc::iovec_array hahaha;
        ctrl_buffer_array():a(999999){
            ERPC_INFO("ctrl_buffer_array constructure\n");
        }
        ~ctrl_buffer_array(){
        }
        static void* create(){
            return static_cast<void* >(new ctrl_buffer_array());
        }
        void destory(){
            delete this;
        }
        static ctrl_buffer_array* deserialize (struct iovector* vec){
            Deserializer deser;
            return deser.deserialize<ctrl_buffer_array>(vec);
        }
        PROCESS_FIELDS(hahaha);
    };

}