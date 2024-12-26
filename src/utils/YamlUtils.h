#ifndef YAML_UTILS_H
#define YAML_UTILS_H
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

// 使用 nlohmann::json 作为 JSON 数据结构
using json = nlohmann::json;

// 递归函数，将 JSON 对象转换为 YAML 节点
// YAML::Node jsonToYaml(const json &j);
// json yamlToJson(const YAML::Node &node);

inline void get_yaml_node(const std::string config_path, YAML::Node& yaml_node){
    std::ifstream yamlFile(config_path);
    if (!yamlFile.is_open()) {
        // spdlog::error("can't open {} file.", config_path);
        std::cerr << "can't open ["<< config_path <<"] file." << std::endl; 
        return ;
    }
    yaml_node = YAML::Load(yamlFile);
}


// 递归函数，将 JSON 对象转换为 YAML 节点
inline YAML::Node jsonToYaml(const json &j) {
    if (j.is_null()) {
        return YAML::Node();
    } else if (j.is_boolean()) {
        return YAML::Node(j.get<bool>());
    } else if (j.is_number()) {
        if (j.is_number_integer()) {
            return YAML::Node(j.get<int>());
        } else {
            return YAML::Node(j.get<double>());
        }
    } else if (j.is_string()) {
        return YAML::Node(j.get<std::string>());
    } else if (j.is_array()) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (const auto &element : j) {
            node.push_back(jsonToYaml(element));
        }
        return node;
    } else if (j.is_object()) {
        YAML::Node node(YAML::NodeType::Map);
        for (json::const_iterator it = j.begin(); it != j.end(); ++it) {
            node[it.key()] = jsonToYaml(it.value());
        }
        return node;
    }
    return YAML::Node();
}

// 递归函数，将 YAML 节点转换为 JSON 对象
inline json yamlToJson(const YAML::Node &node) {
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

inline void get_log_cfg_json(const std::string config_path, nlohmann::json& log_cfg_json){
    YAML::Node yaml_node; 
    get_yaml_node(config_path, yaml_node); 
    log_cfg_json = yamlToJson(yaml_node); 
}

#endif // YAML_UTILS_H
