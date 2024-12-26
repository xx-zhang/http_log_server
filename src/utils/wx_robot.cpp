#include "wx_robot.h"
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// #include <regex>

// bool get_server_host(std::string url, std::string &server_host, std::string &uri) {
//     std::regex url_regex(R"((\w+://[^/]+)(/.*))");

//     std::smatch url_match;
//     if (std::regex_match(url, url_match, url_regex)) {
//         std::string server_host = url_match[1].str(); // 提取前面一部分 (scheme和host)
//         std::string uri = url_match[2].str();         // 提取后面一部分 (path、query和fragment)
//         return true;
//     } else {
//         return false;
//     }
// }

WxRobot::WxRobot(const std::string &webhook_key) :
    webhook_key_(webhook_key), client_("https://qyapi.weixin.qq.com") {
}

WxRobot::~WxRobot() = default;

bool WxRobot::send_text(const std::string &content) {
    json j = {
        {"msgtype", "text"},
        {"text", {{"content", content}}}};

    return send_message(j);
}

bool WxRobot::send_markdown(const std::string &content) {
    json j = {
        {"msgtype", "markdown"},
        {"markdown", {{"content", content}}}};

    return send_message(j);
}

bool WxRobot::send_image(const std::string &base64_img, const std::string &md5_img) {
    json j = {
        {"msgtype", "image"},
        {"image", {{"base64", base64_img}, {"md5", md5_img}}}};

    return send_message(j);
}

bool WxRobot::send_message(const nlohmann::json &j) {
    httplib::Headers headers = {
        {"Content-Type", "application/json"}};
    // client_.set_connection_timeout(3);
    auto res = client_.Post("/cgi-bin/webhook/send?key=" + webhook_key_, j.dump(), "application/json");

    if (res && res->status == 200) {
        auto response = json::parse(res->body);
        if (response["errcode"] == 0) {
            return true;
        } else {
            std::cerr << "Error sending message: " << response["errmsg"] << std::endl;
        }
    } else {
        std::cerr << "Error sending message: " << res->status << std::endl;
    }

    return false;
}
