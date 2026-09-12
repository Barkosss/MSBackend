#include "downloadHandler.h"

#include <iostream>

void downloadHandler(const Request& request, Response& response) {
    std::string body = request.body();
    
    std::cout << body << std::endl;
}