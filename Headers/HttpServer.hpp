#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <asio.hpp>
#include <memory>
#include <filesystem>
#include "PrepareAnswer.hpp"

class HttpServer
{
    public:
        HttpServer(asio::io_context& io_context, int port = 23457);

        void start();

    private:
        void doAccept();

        asio::ip::tcp::acceptor acceptor_;
        PrepareAnswer prepareAnswer;
};

#endif // HTTP_SERVER_HPP