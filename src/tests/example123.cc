#include <cerrno>
#include <cstring>
#include <fstream>
#include "iostream"
#include <map>
#include <random>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"
// #include "yaml-cpp/yaml.h"

#include "utils/JsonUtils.h"
#include "utils/YamlUtils.h"

void beauty_map(const std::map<std::string, std::string> result ) {
    for(auto x: result){
        std::cout << x.first << "=" << x.second << std::endl; 
    }
}

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

void test1(const std::string log_path, std::map<std::string, std::string>& result){
    nlohmann::json json_data; 
    get_random_json_data(log_path, json_data); 
    flatten_json(json_data, "", result); 
    beauty_map(result); 
}


// 这个配置也可以写道yaml里面； 
static std::map<std::string, std::string> rule3_category_map = {
    {"717", "自定义补丁规则"},  
    {"900", "示例规则"}, 
    {"901", "默认引擎初始化规则"}, 
    {"905", "通用请求安全配置规则"}, 
    {"911", "强制方法限制"}, 
    {"913", "扫描器检测"}, 
    {"920", "协议合规检测"}, 
    {"921", "协议攻击检测"}, 
    {"922", "混合攻击检测"}, 
    {"930", "远程文件包含检测"}, 
    {"932", "远程命令执行检测"}, 
    {"933", "PHP代码检测"}, 
    {"934", "常用应用攻击检测"}, 
    {"941", "XSS注入检测"}, 
    {"942", "SQL注入检测"}, 
    {"943", "会话异常检测"}, 
    {"944", "JAVA代码检测"}, 
    {"949", "请求阶段BLOCK计分系统"}, 
    {"950", "数据泄露检测"}, 
    {"951", "SQL信息泄露检测"}, 
    {"952", "JAVA信息泄露检测"}, 
    {"953", "PHP信息泄露检测"}, 
    {"954", "IIS中间件信息泄露检测"}, 
    {"955", "WebShell内容检测"}, 
    {"959", "响应阶段BLOCK计分系统"}, 
    {"980", "上下文计分系统"}, 
    {"999", "Example示例测试规则"}, 
};


void generate_rule_category(const nlohmann::json messages_data, nlohmann::json& rule_map) {
    // 整个message_data 
    if (! messages_data.is_array()){
        return; 
    }
    std::string rule_id; 
    std::string rule_category; 
    std::string rule_msg; 
    int rule_severity = 3; 

    bool allow_overide = true; 
    // attack_content_1_2_3 
    int index = 0; 
    for (auto message: messages_data) {
        index +=1 ; 
        if(allow_overide){
            if(message.contains("data")) {
                auto msg_data = message["data"]; 
                if (msg_data.contains("id")){
                    auto i_rule_id = msg_data["id"].get<int>(); 
                    auto rule_id_pre3 = std::to_string(i_rule_id).substr(0, 3); 
                    if(rule_id_pre3 == "920" or rule_id_pre3 == "901" ) 
                        continue; 
                    if (std::stoi(rule_id_pre3) < 949 or std::stoi(rule_id_pre3) == 717) {
                        rule_id = std::to_string(i_rule_id); 
                        rule_msg = msg_data.contains("msg") ? msg_data["msg"].get<std::string>(): "-"; 
                        rule_severity = msg_data.contains("severity") ? msg_data["severity"].get<int>() :3; 
                        rule_category = rule3_category_map.find(rule_id_pre3) != rule3_category_map.end() ? \
                            rule3_category_map[rule_id_pre3] : "-"; 
                        allow_overide = false; 
                    }   
                }
            }

        }

        std::vector<std::string > key_vec = {"msg", "data", "id"};
        for(auto x : key_vec){
            rule_map["ext_alert_"+ x +"_" + std::to_string(index)] = \
            message["data"].contains(x) ? \
            message["data"][x]: "-"; 
        }
    }
    rule_map["rule_id"] = rule_id; 
    rule_map["rule_category"] = rule_category; 
    rule_map["rule_msg"] = rule_msg; 
    rule_map["rule_severity"] = rule_severity; 

}


void get_apisix_http_query_string(const nlohmann::json http_query, nlohmann::json& http_query_map){

}

void test_message_data(){
    const std::string log_path = "/opt/hibot/src/data/audit.log.example"; 
    nlohmann::json json_data; 
    get_random_json_data(log_path, json_data); 

    auto messages_data = json_data["messages"]; 
    nlohmann::json rule_map; 
    generate_rule_category(messages_data, rule_map); 

    std::cout << rule_map.dump() << std::endl; 
    
}


void get_config_data(const std::string config_path, const std::string log_type){
    std::map<std::string, std::string> result ;
    // test1("/opt/hibot/src/data/audit.log.example", result); 

    nlohmann::json json_data; 
    get_random_json_data("/opt/hibot/src/data/audit.log.example", json_data); 
    flatten_json(json_data, "", result); 
    // beauty_map(result); 

    std::ifstream yamlFile(config_path);
    if (!yamlFile.is_open()) {
        spdlog::error("can't open {} file.", config_path);
        return ;
    }
    YAML::Node yamlNode = YAML::Load(yamlFile);
    auto cfg_json = yamlToJson(yamlNode); 

    // std::cout << cfg_json.dump() << std::endl; 
    nlohmann::json new_map; 
    auto http_log_config = cfg_json[log_type]; 
    for (const auto &y : http_log_config) {
        // 第二层嵌套的结构体； ---- uuid : {"" }
        for(auto it = y.begin(); it != y.end(); ++it) {
            auto _key = it.key(); 
            auto x = it.value(); 
            if(x.contains("enable") && x["enable"].is_boolean() && x["enable"] == false){
                continue;
            }
            if(x.contains("origin_key")){
                try {
                    // TODO 这个值默认dump了两次不知道什么原因； 这里我们直接合一次。
                    new_map[_key] = nlohmann::json::parse(result[x["origin_key"]]); 
                } catch (...) {
                    new_map[_key] = "-"; 
                }
                continue;
            }
            if(x.contains("const_value")){
                new_map[_key] = x["const_value"]; 
                continue;
            }

            // 这里是调用方法进行key的富华；
            if(x.contains("key_func")){
                
                if (! x["key_func"].is_string()){
                    spdlog::error("key_func 【{}】type ERROR", x["key_func"].dump());
                    continue ; 
                }
                // x["key_func"])
                auto local_key_func_str = x["key_func"].get<std::string>(); 
                if(strcmp(local_key_func_str.c_str(), "generate_rule_category")) {
                    // spdlog::warn("执行了这里， 对于local_func 存在这个内容");
                    if(json_data.contains("messages")){
                        nlohmann::json _rule_map;
                        generate_rule_category(json_data["messages"], _rule_map) ; 
                        // std::cout << _rule_map.dump(); 
                        for(auto it = _rule_map.begin(); it != _rule_map.end(); it++) {
                            if(! new_map.contains(it.key())){
                                new_map[it.key()] = it.value(); 
                            }
                        }
                    }
                }

            }
        }

    }

    std::cout << new_map.dump() << std::endl; 

}

void test3(){
    std::map<std::string, std::string> result ;
    test1("/opt/hibot/src/data/audit.log.example", result); 
    beauty_map(result); 
}

int main() {
    // test1(); 
    // get_config_data("/opt/hibot/src/data/log_waf.yaml"); 
    // test_message_data(); 
    get_config_data("/opt/hibot/src/data/log_waf.yaml", "audit_log"); 

    return 0; 
}

// g++ ./example123.cc -o e.elf -I ../../contrib/include -I ../ -lyaml-cpp  -std=c++11 && ./e.elf 
