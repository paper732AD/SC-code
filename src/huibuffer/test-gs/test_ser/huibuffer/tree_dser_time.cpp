#include <cassert>
#include <chrono>
#include <gflags/gflags.h>
#include <iostream>
#include <ostream>
#include <string>

#include "../../../serialize.h"
#include "tree3.h"
#include "../util/time_cycle.h"
using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1024, "the length of array\n");



Tree2 req;
std::string s;

// std::string s;
leaf get_Leaf(){
    leaf *lf = new leaf();
    lf->a.assign(s);
    return *lf;
}
Tree1 get_tree1(){
    Tree1* res = new Tree1();
    res->Left = get_Leaf();
    res->Right = get_Leaf();
    return *res;
}
Tree2 get_tree2(){
    Tree2* res = new Tree2();
    res->Left = get_tree1();
    res->Right = get_tree1();
    return *res;
}
// // 栈分配内存
// leaf get_Leaf(){
//     leaf lf ;
//     lf.a.assign(s);
//     return lf;
// }
// Tree1 get_tree1(){
//     Tree1 res;
//     res.Left = get_Leaf();
//     res.Right = get_Leaf();
//     return res;
// }
// Tree2 get_tree2(){
//     Tree2 res;
//     res.Left = get_tree1();
//     res.Right = get_tree1();
//     return res;
// }
using namespace erpc;

int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    double total_cycle = 0;
    s = *new std::string(FLAGS_total_len,'h');
    assert(s.length()==FLAGS_total_len);
    for(int i = 0 ; i < FLAGS_times ; i ++){
            SerializerIOV ser;
            req = get_tree2();
            ser.serialize(req);

            DeserializerIOV dser;
            auto t1 = get_cpu_cycle();
            auto dser_data = dser.deserialize<Tree2>(&ser.iov);
            auto t2 = get_cpu_cycle();
            total_cycle += double(t2-t1)/FLAGS_times;


    }
    std :: cout << "serialize total cycles : " << total_cycle << std::endl;
    return 0;
}
