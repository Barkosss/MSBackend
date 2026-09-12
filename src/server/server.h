#pragma once

#include "server/router.h"
#include <boost/asio.hpp>

namespace http = boost::beast::http;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;
using boost::asio::ip::tcp;

class Server {
private:
    tcp::acceptor acceptor;
    uint16_t port;

    void do_accept();
    void handler(const Request& request, Response& response);

public:
    Server(boost::asio::io_context& io_context, uint16_t port): acceptor(io_context, tcp::endpoint(tcp::v4(), port)), port(port) {
        do_accept();
    };
};