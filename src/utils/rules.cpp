#include "rules.h"

#include <iostream>
#include <fstream>

const std::string DEFAULT_RULES_FILE = "rules.json";

Rules::Rules() {
    loadRules(DEFAULT_RULES_FILE);
};

void Rules::loadRules(const std::string& filename) {
    rulesData = nlohmann::json::object();

    std::ifstream rulesFile(filename);
    if (!rulesFile.is_open()) {
        std::cerr << "Could not open rules file: " << filename << std::endl;
        return;
    }

    rulesData = nlohmann::json::parse(rulesFile);
}

Rules& Rules::getInstance() {
    static Rules instance;
    return instance;
}

void Rules::reload() {
    loadRules(DEFAULT_RULES_FILE);
}

const std::vector<std::string>* Rules::splitKey(const std::string& key) const {
    std::vector<std::string>* keys = new std::vector<std::string>();

    size_t start = 0;
    size_t end = key.find('.');

    while (end != std::string::npos) {
        keys->push_back(key.substr(start, end - start));
        start = end + 1;
        end = key.find('.', start);
    }

    keys->push_back(key.substr(start));

    return keys;
}