#include "reloadHandler.h"

#include "utils/config.h"
#include "utils/rules.h"

void reloadHandler(const Request& request, Response& response) {
    Config::getInstance().reload();
    Rules::getInstance().reload();

    response.result(http::status::ok);
    response.set(http::field::content_type, "text/plain");
    response.body() = "The configuration and rules have been reloaded.";
    response.prepare_payload();
}