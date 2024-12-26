#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <string>
#include <vector>
#include "httplib.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class WxRobot {
public:
    explicit WxRobot(const std::string &webhook_key);
    ~WxRobot();

    bool send_text(const std::string &content);
    bool send_markdown(const std::string &content);
    bool send_image(const std::string &base64_img, const std::string &md5_img);
    bool send_message(const nlohmann::json &j);

private:
    std::string webhook_key_;
    httplib::Client client_;

    // bool send_message(const json &j); // 添加send_message方法声明
};
