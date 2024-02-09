#include "HttpConnection.hpp"

HttpConnection::HttpConnection(asio::ip::tcp::acceptor& context, PrepareAnswer& prepareAnswer)
    : socket_(context.get_executor()), prepareAnswer(prepareAnswer) {}

void HttpConnection::start()
{
    doRead();
}

void HttpConnection::doRead()
{
    auto self = shared_from_this();
        socket_.async_read_some(asio::buffer(m_requestBuffer),
            [this, self](std::error_code errorCode, size_t transferredBytesCount)
            {
                if (!errorCode) {
                    // Process the request
                    response = prepareAnswer.exe(std::string(m_requestBuffer.begin(), m_requestBuffer.begin() + transferredBytesCount))->res;

                    // Start writing the response
                    doWrite();
                }
            });
}

void HttpConnection::doWrite()
{
    auto self = shared_from_this();
        asio::async_write(socket_, asio::buffer(response),
            [this, self](std::error_code writeEc, std::size_t /*bytesWritten*/)
            {
                if (!writeEc) {
                    // Close the socket after writing
                    socket_.close();

                    // Continue accepting more connections
                    // Note: You may want to remove the doAccept call if you're handling all connections in a single io_context
                    // doAccept();
                }
            });
}