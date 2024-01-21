#pragma once

#include "../Response/Response.hpp"
#include <string>


class Client
{

    public:
        size_t contentLenght;
        size_t readData;
        int fd;
        int reqType;
        bool firstParse;
        std::map<st_, st_> headers;
        request request;
        Response response;
        bool gotResp;
        Client();
        ~Client();

};

Client::Client() : gotResp(false), firstParse(false), gotResp(false)
{

}
