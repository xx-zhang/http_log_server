#include <cerrno>
#include <cstring>
#include <fstream>
#include "iostream"
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
// #include "yaml-cpp/yaml.h"
#include "httplib.h"
// g++ ./test_sender.cpp -I ../../contrib/include -o sd.elf 

void get_all_json_data(const std::string log_path, const std::string log_prefix){
    // std::ifstream audit_logfile("/opt/hibot/src/data/audit.log.example");
    std::ifstream audit_logfile(log_path);
    char ch;
    std::string line;

    std::vector<nlohmann::json> result_vec; 
    
    if (audit_logfile.is_open()) {
        while (audit_logfile.get(ch)) {
            if (ch == '\n') {
                // std::cout << line << std::endl << std::endl; 
                const auto j = nlohmann::json::parse(line); 
                result_vec.push_back(j); 
                line.clear();
            } else {
                line += ch;
            }
        }
        if (!line.empty()) {
           spdlog::error("LLLLLLLLLLLLLLLLLLLLLL__END");
        }
        audit_logfile.close();
    } else {
        spdlog::error("Unable to open this file");
    }

    for(auto x: result_vec) {
        std::string local_path = log_prefix ; 
        httplib::Headers local_headers = {
            // {"Authorization", "Basic cm9vdEBleGFtcGxlLmNvbTpFSDFsbGt5Q2tGZDJpamJL"}, 
            {"User-Agent", "curl 7.28.1"}, 
            {"Connection",  "close"}, 
            {"Accept", "*/*"}
        };
        httplib::Client cli("http://127.0.0.1:8811");
        auto res = cli.Post(local_path.c_str(), local_headers, x.dump(), "application/json");
        
        // 检查响应是否有效
        if (res && res->status == 200) {
        } else {
            std::cout << "Request failed -> " << res->status << std::endl;
        }
    }
}

void insert_many(const std::string log_path, const std::string log_prefix){
    // std::ifstream audit_logfile("/opt/hibot/src/data/audit.log.example");
    std::ifstream audit_logfile(log_path);
    char ch;
    std::string line;

    nlohmann::json result_vec; 
    
    if (audit_logfile.is_open()) {
        while (audit_logfile.get(ch)) {
            if (ch == '\n') {
                // std::cout << line << std::endl << std::endl; 
                const auto j = nlohmann::json::parse(line); 
                result_vec.push_back(j); 
                line.clear();
            } else {
                line += ch;
            }
        }
        if (!line.empty()) {
           spdlog::error("LLLLLLLLLLLLLLLLLLLLLL__END");
        }
        audit_logfile.close();
    } else {
        spdlog::error("Unable to open this file");
    }

    std::cout << result_vec.dump() << std::endl; 

    // for(auto x: result_vec) {
        std::string local_path = log_prefix ; 
        httplib::Headers local_headers = {
            // {"Authorization", "Basic cm9vdEBleGFtcGxlLmNvbTpFSDFsbGt5Q2tGZDJpamJL"}, 
            {"User-Agent", "curl 7.28.1"}, 
            {"Connection",  "close"}, 
            {"Accept", "*/*"}
        };
        httplib::Client cli("http://127.0.0.1:8811");
        auto res = cli.Post(local_path.c_str(), local_headers, result_vec.dump(), "application/json");
        
        // 检查响应是否有效
        if (res && res->status == 200) {
        } else {
            std::cout << "Request failed -> " << res->body << std::endl;
        }
    // }
}

int main(){
    // const std::string log_path = "/opt/hibot/src/data/audit.log.example"; 
    const std::string log_path = "/opt/hibot/src/data/access.log.example"; 
    // const std::string log_path = "/opt/hibot/src/data/access.log.example";
    // TODO 这里的 prefix需要在 openserve中添加数据流，并且必须yaml中有对应的字符串。 
    std::string log_prefix = "/http_log"; 
    for(int i=0; i < 10; i++){
        // get_all_json_data( log_path,  log_prefix); 
        insert_many(log_path,  log_prefix); 
    }
    return 0; 
}
