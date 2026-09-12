#include "checkHandler.h"

#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>

// curl -X POST http://192.168.1.109:54322/check -H "Content-Type: application/json" -d "{\"name\":\"John\",\"age\":30}"

std::optional<std::string> validateFileSchema(const nlohmann::json& json);

void checkHandler(const Request& request, Response& response) {
    nlohmann::json body = nlohmann::json::parse(request.body());

    if (!body.contains("files")) {
        response.result(http::status::unprocessable_entity);
        response.set(http::field::content_type, "text/plain");
        response.body() = nlohmann::json({
            {"error", "missing_field"},
            {"message", "Required field 'files' is missing"}
        }).dump();
        response.prepare_payload();
        return;
    }

    if (!body["files"].is_array()) {
        response.result(http::status::unprocessable_entity);
        response.set(http::field::content_type, "text/plain");
        response.body() = nlohmann::json({
            {"error", "missing_field"},
            {"message", "Field 'files' must be an array"}
        }).dump();
        response.prepare_payload();
        return;
    }

    std::vector<nlohmann::json> files = body["files"];

    for (const auto& file : files) {
        auto error = validateFileSchema(file);

        if (error) {
            response.result(http::status::unprocessable_entity);
            response.set(http::field::content_type, "application/json");
            response.body() = nlohmann::json({
                {"error", "invalid_structure"},
                {"message", *error}
            }).dump();
            response.prepare_payload();
            return;
        }
    }

}

std::optional<std::string> validateFileSchema(const nlohmann::json& json) {
    // TODO: Improve the JSON structure validation.
    if (!json.is_object()) {
        return "Each item in 'files' must be an object";
    }

    if (!json.contains("filename")) {
        return "Required field 'filename' is missing";
    }

    if (!json["filename"].is_string()) {
        return "Field 'filename' must be a string";
    }

    if (!json.contains("hash")) {
        return "Required field 'hash' is missing";
    }

    if (!json["hash"].is_string()) {
        return "Field 'hash' must be a string";
    }

    if (!json.contains("size")) {
        return "Required field 'size' is missing";
    }

    if (!json["size"].is_number_unsigned()) {
        return "Field 'size' must be a non-negative number";
    }

    if (!json.contains("chunkCount")) {
        return "Required field 'chunkCount' is missing";
    }

    if (!json["chunkCount"].is_number_unsigned()) {
        return "Field 'chunkCount' must be a non-negative number";
    }

    if (!json.contains("chunkHashes")) {
        return "Required field 'chunkHashes' is missing";
    }

    if (!json["chunkHashes"].is_array()) {
        return "Field 'chunkHashes' must be an array";
    }

    for (const auto& chunk : json["chunkHashes"]) {
        if (!chunk.is_string()) {
            return "Every item in 'chunkHashes' must be a string";
        }
    }

    return std::nullopt;
}
