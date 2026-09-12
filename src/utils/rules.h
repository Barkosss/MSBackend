#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class Rules {
private:
    nlohmann::json rulesData;

    Rules();
    void loadRules(const std::string& filename);
    const std::vector<std::string>* splitKey(const std::string& key) const;

public:
    static Rules& getInstance();
    void reload();

    template<typename Type>
    Type get(const std::string& key) const {
        const std::vector<std::string>* keys = splitKey(key);

        const nlohmann::json* current = &rulesData;

        for (const std::string& _key : *keys) {
            if (current->contains(_key)) {
                current = &(*current)[_key];
            } else {
                return Type();
            }
        }
        delete keys;

        return current->get<Type>();
    }
};