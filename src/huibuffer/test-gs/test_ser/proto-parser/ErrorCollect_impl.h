//
// Created by gs on 2021/11/10.
//

#ifndef PROTOBUF_ERRORCOLLECT_IMPL_H
#define PROTOBUF_ERRORCOLLECT_IMPL_H


class MockErrorCollector : public google::protobuf::io::ErrorCollector {
public:
    MockErrorCollector(){
        warning_ = "no";
        text_ = "no";
    };
    ~MockErrorCollector() override = default;;

    std::string warning_;
    std::string text_;

    // implements ErrorCollector ---------------------------------------
    void AddWarning(int line, int column, const std::string& message) override {
        printf("warning!\n");
//        strings::SubstituteAndAppend(&warning_, "$0:$1: $2\n", line, column, message);
    }

    void AddError(int line, int column, const std::string& message) override {
//        strings::SubstituteAndAppend(&text_, "$0:$1: $2\n", line, column, message);
        printf("Error!\n");
    }
};

#endif //PROTOBUF_ERRORCOLLECT_IMPL_H
