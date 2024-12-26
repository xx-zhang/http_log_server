#include "utils/coraza_log_helper.h"

#include "random"


void get_random(const int max_value, int& random_value){
    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(1, max_value);
    random_value =  distribution(generator); 
}

void get_random_json_data(const std::string log_path, nlohmann::json& json_data){
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

    int random_value; 
    get_random(result_vec.size(), random_value); 
    json_data = result_vec[random_value]; 
}

nlohmann::json log_cfg_json; 

nlohmann::json parse_post_data_standard(nlohmann::json json_data, const std::string log_prefix)
{
    auto log_type = log_prefix.substr(1, log_prefix.size());
    
    auto x = parse_log_standard(log_cfg_json, json_data, log_type); 
    // std::cout << x.dump() << std::endl;  // 这里是调试结果输出时候用；
    return x; 
}

int main() {
    // nlohmann::json cfg_json; 
    // YAML::Node yaml_node; 
    // get_yaml_node("/opt/hibot/src/data/log_waf.yaml", yaml_node) ; 
    // cfg_json = yamlToJson(yaml_node); 
    get_log_cfg_json("/opt/hibot/src/data/log_waf.yaml", log_cfg_json);

    
    nlohmann::json json_data; 
    get_random_json_data( "/opt/hibot/src/data/access.log.example", json_data) ; 
    // auto new_map = parse_log_standard(cfg_json, json_data,"http_log");
    auto new_map = parse_post_data_standard(json_data, "/http_log");  
    std::cout << new_map.dump() << std::endl; 
    return 0; 
}

// g++ ./coraza_logtest.cpp -o e.elf -I ../../contrib/include -I ../ -lyaml-cpp  -std=c++11 && ./e.elf 
