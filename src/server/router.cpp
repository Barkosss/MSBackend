#include "router.h"

#include <iostream>

Router& Router::getInstance() {
    static Router instance;
    return instance;
}

void Router::addRoute(http::verb verb, std::string path, Handler handler) {
    if (routes.find(verb) == routes.end()) {
        routes[verb] = {};
    }

    if (routes[verb].find(path) != routes[verb].end()) {
        std::cerr << "Warning: The path " << path << " is duplicate with verb " << verb << std::endl;
        return;
    }

    std::cout << "Adding route: " << path << " with verb " << verb << std::endl;
    routes[verb][path] = handler;
}

const Handler& Router::getHandler(http::verb verb, std::string& path) {
    if (routes.find(verb) == routes.end()) {
        return {};
    }

    if (routes[verb].find(path) == routes[verb].end()) {
        return {};
    }

    return routes[verb][path];
}

bool Router::hasRoute(http::verb verb, std::string& path) {
    if (routes.find(verb) == routes.end()) {
        return false;
    }

    if (routes[verb].find(path) == routes[verb].end()) {
        return false;
    }

    return true;
}