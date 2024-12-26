#include "yaml-cpp/yaml.h"

#include "stdio.h"
#include <string>
#include <iostream>
// #include <type_traits>
#include <sstream>
#include "nlohmann/json.hpp"

// 将YAML文档转换为JSON
nlohmann::json yamlToJson(const YAML::Node &node) {
    nlohmann::json json;

    for (auto it = node.begin(); it != node.end(); ++it) {
        if (it->second.Type() == YAML::NodeType::Scalar) {
            json[it->first.as<std::string>()] = it->second.as<std::string>();
        } else if (it->second.Type() == YAML::NodeType::Sequence) {
            auto seq = it->second;
            for (int i = 0; i < seq.size(); ++i) {
                json[it->first.as<std::string>()].push_back(seq[i].as<std::string>());
            }
        } else if (it->second.Type() == YAML::NodeType::Map) {
            json[it->first.as<std::string>()] = yamlToJson(it->second);
        }
    }

    return json;
}

template <class T>
static inline void find(YAML::Node config, const std::string &type, T &val) {
    if constexpr (std::is_same<T, YAML::Node>::value) {
        if (config.Type() != YAML::NodeType::value::Scalar && (config[type].Type() == YAML::NodeType::value::Sequence || config[type].Type() == YAML::NodeType::value::Map)) {
            val = config[type];
            return;
        }
    } else {
        if (config.Type() != YAML::NodeType::value::Scalar && config[type].Type() == YAML::NodeType::value::Scalar) {
            val = config[type].as<T>();
            return;
        }
    }

    switch (config.Type()) {
    case YAML::NodeType::value::Undefined:
    case YAML::NodeType::value::Null:
    case YAML::NodeType::value::Scalar: // scalar
    {
        break;
    }
    case YAML::NodeType::value::Sequence: // sequence
    {
        for (auto itvv : config) {
            find<T>(itvv, type, val);
        }
        break;
    }
    case YAML::NodeType::value::Map: // map
    {
        for (auto itvv : config) {
            YAML::Node nNext = itvv.second;
            find<T>(nNext, type, val);
        }
        break;
    }
    default:
        break;
    }
}

YAML::Node load(const std::string &file) {
    printf("load %s.\n", file.c_str());

    YAML::Node config = YAML::LoadFile(file);
    if (config.Type() == YAML::NodeType::value::Undefined || config.Type() == YAML::NodeType::value::Null || config.Type() == YAML::NodeType::value::Scalar) {
        printf("error load %s.\n", file.c_str());
    }

    return config;
}

int main(int argc, char *argv[]) {
    YAML::Node cfg = load("./log_map.yaml");
    std::cout << cfg << std::endl;
    // std::string http_user_agent
    auto http_log_node = cfg["http_log"];
    for (auto it = http_log_node.begin(); it != http_log_node.end(); ++it) {
    }
    auto x = yamlToJson(cfg);
    std::cout << x.dump() << std::endl;

    // find(cfg, "origin_key", http_user_agent);
    // std::cout << http_user_agent << std::endl;
    // std::string Q1, Q2, Q3;
    // find(cfg, "param", Q1);
    // find(cfg, "Id1", Q2);
    // find(cfg, "S1", Q3);
    // printf("Q1:%s,Q2:%s,Q3:%s\n", Q1.c_str(), Q2.c_str(), Q3.c_str());
    return 0;
}
