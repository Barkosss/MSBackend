#include "manifestHandler.h"

#include "utils/rules.h"
#include "utils/config.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::string> getHash(const std::string& filePath);

void manifestHandler(const Request& request, Response& response) {
    std::string modsPath = Config::getInstance().get<std::string>("mods.directory");
    const uint32_t CHUNK_SIZE = Config::getInstance().getChunkSize();

    if (modsPath.empty()) {
        response.result(http::status::internal_server_error);
        response.set(http::field::content_type, "text/plain");
        response.body() = "The server has no configuration set up.";
        response.prepare_payload();
        return;
    }

    std::vector<nlohmann::json> manifestFiles;

    for (const auto& file : fs::directory_iterator(modsPath)) {
        nlohmann::json manifestFile;
        manifestFile["name"] = file.path().filename().string();
        manifestFile["path"] = file.path().string();
        manifestFile["hash"] = std::hash<std::string>{}(file.path().string());
        manifestFile["size"] = fs::file_size(file.path());

        std::vector<std::string> chunkHashes = getHash(file.path().string());
        manifestFile["chunkCount"] = chunkHashes.size();
        manifestFile["chunkHashes"] = chunkHashes;

        manifestFiles.push_back(manifestFile);
    }

    nlohmann::json manifest;
    manifest["chunk_size"] = CHUNK_SIZE;
    manifest["files"] = manifestFiles;

    response.result(http::status::ok);
    response.set(http::field::content_type, "application/json");
    response.body() = manifest.dump();
    response.prepare_payload();
}

std::vector<std::string> getHash(const std::string& filePath) {
    const uint32_t CHUNK_SIZE = Config::getInstance().getChunkSize();
    std::vector<std::string> chunkHashes;

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return chunkHashes;
    }

    std::vector<char> buffer(CHUNK_SIZE);
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        std::string chunkData(buffer.data(), file.gcount());
        std::string chunkHash = std::to_string(std::hash<std::string>{}(chunkData));
        chunkHashes.push_back(chunkHash);
    }

    file.close();
    return chunkHashes;
}
