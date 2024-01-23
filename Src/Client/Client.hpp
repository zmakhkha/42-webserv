#pragma once

#include "../Response/Response.hpp"

class Client {
public:
    request req;
    Response resp;
    bool gotResponse;

    Client() : req(), resp(), gotResponse(false) {}
};