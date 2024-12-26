
#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <string>
#include <map>
#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>

inline void flatten_json(const nlohmann::json &j, const std::string &prefix, std::map<std::string, std::string> &result) {
    if (j.is_array()) {
        for (size_t i = 0; i < j.size(); ++i) {
            flatten_json(j[i], prefix + "." + std::to_string(i), result);
        }
        return;
    }

    if (!j.is_object()) {
        result[prefix] = j.dump(); 
        return;
    }

    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        flatten_json(*it, key, result);
    }
}

struct http_log {
    std::string uuid;
    std::string client_ip;
    std::string http_upstream;
    std::string http_method;
    std::string http_url; // 完整的URL -> "http://127.0.0.1:1984/hello?log_body=no",
    std::string http_uri; // 只有个简易的路由URL -> /hello?
    std::string domain;
    std::string http_host;
    std::string http_version;
    std::string http_xff;
    std::string http_referer;
    std::string content_type;
    std::string content_length;
    std::string resp_content_length;
    std::string http_user_agent;
    std::string http_request_body;
    std::string http_request_headers_raw;
    std::string http_response_headeres_raw;
    std::string http_response_body;
    std::string http_query;
    std::string http_cookie;
    int status;
    int upstream_status;
    size_t sent_size;
    size_t recv_size;
    std::string request_body;

    // --- 下面是富华信息，不一定携带了
    std::string ipinfo_country;
    std::string ipinfo_city;
    std::string ipinfo_isp;
};

struct coraza_waf_log {
    std::string uuid;
    std::string attack_time; // 需要修改为timestamp
    std::string rule_id;
    std::string attack_place;    // 命中的位置。
    std::string attack_content;  // 命中详情
    std::string attack_category; // attack_type
    int status;
    std::string http_url;

    std::string http_request_body;
    std::string http_request_raw; // 合并request headers + http body 作为一个大body
    std::string http_host;        // host
    std::string client_ip;
    std::string risk_level;
    std::string http_method;
    std::string http_user_agent;
    // --- 下面是富华信息，不一定携带了
    std::string ipinfo_country;
    std::string ipinfo_city;
    std::string ipinfo_isp;
    // std::string ipinfo_nation;
};
// main调用可以前往进行 tests目录里面 查看；

#endif
