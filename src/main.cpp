#include <iostream>
#include <functional>

#include "utils/config.h"

#include "server/router.h"
#include "server/server.h"

#include "handlers/healthzHandler.h"
#include "handlers/manifestHandler.h"
#include "handlers/downloadHandler.h"
#include "handlers/checkHandler.h"
#include "handlers/reloadHandler.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    uint16_t port = 0;

    if (argc > 1) {
        try {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        } catch (const std::exception& ex) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }

    if (!port && Config::getInstance().has("server.port")) {
        port = Config::getInstance().get<uint16_t>("server.port");
    }

    if (!port) {
        std::cerr << "Port number not specified. Please provide a port number as a command-line argument or in the config file." << std::endl;
        return 1;
    }

    try {
        boost::asio::io_context io_context{};

        Router::getInstance().addRoute(http::verb::get, "/healthz", healthzHandler);
        Router::getInstance().addRoute(http::verb::get, "/manifest", manifestHandler);
        Router::getInstance().addRoute(http::verb::post, "/download", downloadHandler); // TODO
        Router::getInstance().addRoute(http::verb::post, "/check", checkHandler); // TODO
        Router::getInstance().addRoute(http::verb::get, "/reload", reloadHandler);

        Server server(io_context, port);

        std::cout << "Server is running on port (:" << port << ")" << std::endl;
        io_context.run();

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
