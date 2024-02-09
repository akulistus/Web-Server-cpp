#include "HttpServer.hpp"
#include "HttpConnection.hpp"

HttpServer::HttpServer(asio::io_context& io_context, int port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

void HttpServer::start()
{
    doAccept();
}

void HttpServer::doAccept()
{
    auto connection = std::make_shared<HttpConnection>(acceptor_, prepareAnswer);

        acceptor_.async_accept(connection->socket(),
            [this, connection](std::error_code ec)
            {
                if (!ec) {
                    // Create a new connection and start handling it
                    connection->start();
                }

                // Continue accepting more connections
                doAccept();
            });
}