#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

// 使用 nlohmann::json 作为 JSON 数据结构
using json = nlohmann::json;

// 递归函数，将 YAML 节点转换为 JSON 对象
json yamlToJson(const YAML::Node &node) {
    if (node.IsScalar()) {
        return node.as<std::string>();
    } else if (node.IsSequence()) {
        json j = json::array();
        for (const auto &element : node) {
            j.push_back(yamlToJson(element));
        }
        return j;
    } else if (node.IsMap()) {
        json j;
        for (const auto &element : node) {
            j[element.first.as<std::string>()] = yamlToJson(element.second);
        }
        return j;
    }
    return nullptr;
}

int main() {
    // 读取 YAML 文件
    std::ifstream yamlFile("input.yaml");
    if (!yamlFile.is_open()) {
        std::cerr << "无法打开文件 input.yaml" << std::endl;
        return 1;
    }

    // 解析 YAML 文件
    YAML::Node yamlNode = YAML::Load(yamlFile);

    // 转换为 JSON
    json jsonObject = yamlToJson(yamlNode);

    // 输出 JSON 到控制台
    std::cout << jsonObject.dump(4) << std::endl; // 4 是缩进空格数

    return 0;
}
