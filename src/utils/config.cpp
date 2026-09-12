#include "config.h"

#include <iostream>
#include <optional>

const std::string DEFAULT_CONFIG_FILE = "config.yaml";

Config::Config() {
    loadConfig(DEFAULT_CONFIG_FILE);
};

void Config::loadConfig(const std::string& filename) {
    configData = YAML::LoadFile(filename);

    if (configData.IsNull()) {
        std::cerr << "Failed to load config file: " << filename << std::endl;
        return;
    }
}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

void Config::reload() {
    loadConfig(DEFAULT_CONFIG_FILE);
}

uint32_t Config::getChunkSize() const {
    return get<uint32_t>("update.chunk_size");
}

bool Config::has(const std::string& key) const {
    const std::vector<std::string> keys = splitKey(key);
    YAML::Node current = YAML::Clone(configData);

    for (const std::string& _key : keys) {
        if (!current.IsMap() || !current[_key]) {
            return false;
        }

        current = current[_key];
    }

    return true;
}

const std::vector<std::string> Config::splitKey(const std::string& key) const {
    std::vector<std::string> keys;

    size_t start = 0;
    size_t end = key.find('.');

    while (end != std::string::npos) {
        keys.push_back(key.substr(start, end - start));
        start = end + 1;
        end = key.find('.', start);
    }

    keys.push_back(key.substr(start));

    return keys;
}