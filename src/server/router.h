#pragma once

#include <map>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;
using Handler = std::function<void(const Request& req, Response& res)>;

class Router {
private:
    std::map<http::verb, std::map<std::string, Handler>> routes;

public:
    static Router& getInstance();

    void addRoute(http::verb verb, std::string path, Handler handler);
    const Handler& getHandler(http::verb verb, std::string& path);
    bool hasRoute(http::verb verb, std::string& path);
};