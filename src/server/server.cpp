#include "server.h"

#include <iostream>
#include <boost/url.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

void Server::do_accept() {
    acceptor.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                boost::beast::flat_buffer buffer;

                http::request<http::string_body> req;
                http::read(socket, buffer, req);

                http::response<http::string_body> res;

                handler(req, res);

                http::write(socket, res);
            }

            do_accept();
        }
    );
}

void Server::handler(const Request& request, Response& response) {
    boost::urls::url_view parsed_url(request.target());
    auto path = parsed_url.path();
    
    if (!Router::getInstance().hasRoute(request.method(), path)) {
        response.result(http::status::not_implemented);
        response.set(http::field::content_type, "text/plain");
        response.body() = "Handler is not implemented yet.";
        response.prepare_payload();
        return;
    }

    Router::getInstance().getHandler(request.method(), path)(request, response);
}
