#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <asio.hpp>
#include <memory>
#include "PrepareAnswer.hpp"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
    public:
        HttpConnection(asio::ip::tcp::acceptor& context, PrepareAnswer& prepareAnswer);

        void start();

        asio::ip::tcp::socket& socket() {
        return socket_;
        }

    private:
        void doRead();
        void doWrite();

        asio::ip::tcp::socket socket_;
        PrepareAnswer& prepareAnswer;
        std::string response;
        std::array<char, 32 * 1024> m_requestBuffer;
};

#endif // HTTP_CONNECTION_HPP