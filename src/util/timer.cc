#include <vector>
#include "util/timer.h"
#include "util/logger.h"
#include <algorithm>

namespace erpc{
    
    std::vector<tmpoint> lat_client;
    std::vector<double> latency;
    std::vector<double> after_serialize;
    std::vector<double> after_init;
    std::vector<double> first_send;
    std::vector<double> before_send;
    std::vector<double> first_receive;
    std::vector<double> at_receive;
    std::vector<double> first_expl_cr;
    std::vector<double> last_expl_cr;

    std::atomic<int> lat_client_count;
    bool record_time_flag;
    //TODO 这里需要将使用lat_client来计算换成has_send和has_recv来计算，
    //TODO 因为不是每个请求都只有一个包，两个包，三个包也有可能。

    double _50lat(std::vector<double>& temp) { return temp[uint32_t(0.5 * temp.size())]; }
    double _90lat(std::vector<double>& temp) { return temp[uint32_t(0.9 * temp.size())]; }
    double _95lat(std::vector<double>& temp) { return temp[uint32_t(0.95 * temp.size())]; }
    double _99lat(std::vector<double>& temp) { return temp[uint32_t(0.99 * temp.size())]; }
    double _avglat(std::vector<double>& temp) {
        double sum = 0;
        for (auto lat : temp) {
            sum += lat;
        }
        return sum / temp.size();
    }
    
    void write_detail_to_file(std::ofstream& outfile, std::vector<double>& _latency){
        outfile<<"[ ";
        for(int i=0;i<_latency.size();++i){
            outfile<< _latency[i] ;
            if(i!=_latency.size()-1)
            outfile << ", ";
        }
        outfile << " ]";
    }

    void out_serialize(std::ofstream& outfile, int num=0){
        ERPC_WARN("there are %d records, and after_serialize.size() = %d\n", num, after_serialize.size());

        sort(after_serialize.begin(), after_serialize.end());

        outfile <<"out_serialize latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(after_serialize)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(after_serialize)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(after_serialize)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(after_serialize)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(after_serialize)/1000 <<"    us" << std::endl;
    }

    void out_before_send(std::ofstream& outfile, int num=0){
        ERPC_WARN("there are %d records, and before_send.size() = %d\n", num, before_send.size());

        sort(before_send.begin(), before_send.end());

        outfile <<"out_before_send latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(before_send)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(before_send)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(before_send)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(before_send)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(before_send)/1000 <<"    us" << std::endl;
    }

    void out_at_receive(std::ofstream& outfile, int num=0){
        ERPC_WARN("there are %d records, and at_receive.size() = %d\n", num, at_receive.size());

        sort(at_receive.begin(), at_receive.end());

        outfile <<"out_at_receive latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(at_receive)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(at_receive)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(at_receive)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(at_receive)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(at_receive)/1000 <<"    us" << std::endl;
    }

    void out_init_time(std::ofstream& outfile, int num=0){
        ERPC_WARN("there are %d records, and after_init.size() = %d\n", num, after_init.size());

        sort(after_init.begin(), after_init.end());
        outfile <<"out_init_time latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(after_init)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(after_init)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(after_init)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(after_init)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(after_init)/1000 <<"    us" << std::endl;
    }
    
    void output_latency(std::ofstream& outfile, int num=0){

        sort(latency.begin(), latency.end());
        ERPC_WARN("there are %d records, and latency.size() = %d\n", num, latency.size());

        outfile <<"total latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(latency)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(latency)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(latency)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(latency)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(latency)/1000 <<"    us" << std::endl;
    }

    void out_first_send(std::ofstream& outfile, int num=0){

        sort(first_send.begin(), first_send.end());
        ERPC_WARN("there are %d records, and first_send.size() = %d\n", num, first_send.size());

        outfile <<"first_send latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(first_send)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(first_send)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(first_send)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(first_send)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(first_send)/1000 <<"    us" << std::endl;
    }


    void out_first_receive(std::ofstream& outfile, int num=0){

        sort(first_receive.begin(), first_receive.end());
        ERPC_WARN("there are %d records, and first_receive.size() = %d\n", num, first_receive.size());

        outfile <<"first_receive latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(first_receive)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(first_receive)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(first_receive)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(first_receive)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(first_receive)/1000 <<"    us" << std::endl;
    }

    void out_first_expl_cr(std::ofstream& outfile, int num=0){

        sort(first_expl_cr.begin(), first_expl_cr.end());
        ERPC_WARN("there are %d records, and first_expl_cr.size() = %d\n", num, first_expl_cr.size());

        outfile <<"first_expl_cr latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(first_expl_cr)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(first_expl_cr)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(first_expl_cr)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(first_expl_cr)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(first_expl_cr)/1000 <<"    us" << std::endl;
    }


    void out_last_expl_cr(std::ofstream& outfile, int num=0){

        sort(last_expl_cr.begin(), last_expl_cr.end());
        ERPC_WARN("there are %d records, and last_expl_cr.size() = %d\n", num, last_expl_cr.size());

        outfile <<"last_expl_cr latency = " << std::endl;
        outfile <<"\tavge latency = "  << _avglat(last_expl_cr)/1000 << "    us" << std::endl;
        outfile <<"\t50th latency = " << _50lat(last_expl_cr)/1000 <<"    us" << std::endl;
        outfile <<"\t90th latency = " << _90lat(last_expl_cr)/1000 <<"    us" << std::endl;
        outfile <<"\t95th latency = " << _95lat(last_expl_cr)/1000 <<"    us" << std::endl;
        outfile <<"\t99th latency = " << _99lat(last_expl_cr)/1000 <<"    us" << std::endl;
    }

    // void out_first_rfr(std::ofstream& outfile, int num=0){

    //     sort(first_rfr.begin(), first_rfr.end());
    //     ERPC_WARN("there are %d records, and first_rfr.size() = %d\n", num, first_rfr.size());

    //     outfile <<"first_rfr latency = " << std::endl;
    //     outfile <<"\tavge latency = "  << _avglat(first_rfr)/1000 << "    us" << std::endl;
    //     outfile <<"\t50th latency = " << _50lat(first_rfr)/1000 <<"    us" << std::endl;
    //     outfile <<"\t90th latency = " << _90lat(first_rfr)/1000 <<"    us" << std::endl;
    //     outfile <<"\t95th latency = " << _95lat(first_rfr)/1000 <<"    us" << std::endl;
    //     outfile <<"\t99th latency = " << _99lat(first_rfr)/1000 <<"    us" << std::endl;
    // }


    // void out_last_rfr(std::ofstream& outfile, int num=0){

    //     sort(last_rfr.begin(), last_rfr.end());
    //     ERPC_WARN("there are %d records, and last_rfr.size() = %d\n", num, last_rfr.size());

    //     outfile <<"last_rfr latency = " << std::endl;
    //     outfile <<"\tavge latency = "  << _avglat(last_rfr)/1000 << "    us" << std::endl;
    //     outfile <<"\t50th latency = " << _50lat(last_rfr)/1000 <<"    us" << std::endl;
    //     outfile <<"\t90th latency = " << _90lat(last_rfr)/1000 <<"    us" << std::endl;
    //     outfile <<"\t95th latency = " << _95lat(last_rfr)/1000 <<"    us" << std::endl;
    //     outfile <<"\t99th latency = " << _99lat(last_rfr)/1000 <<"    us" << std::endl;
    // }

}