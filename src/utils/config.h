#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

class Config {
private:
    YAML::Node configData;

    Config();
    void loadConfig(const std::string& filename);
    const std::vector<std::string> splitKey(const std::string& key) const;

public:
    static Config& getInstance();
    void reload();
    uint32_t getChunkSize() const;

    bool has(const std::string& key) const;

    template<typename Type>
    Type get(const std::string& key) const {
        const auto keys = splitKey(key);
        YAML::Node current = YAML::Clone(configData);

        for (const std::string& _key : keys) {
            if (!current.IsMap() || !current[_key]) {
                return Type{};
            }
            
            current = current[_key];
        }

        return current.as<Type>();
    }
};