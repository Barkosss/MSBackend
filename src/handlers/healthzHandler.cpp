#include "healthzHandler.h"

void healthzHandler(const Request& request, Response& response) {
    response.result(http::status::ok);
    response.set(http::field::content_type, "text/plain");
    response.body() = "OK";
    response.prepare_payload();
}
