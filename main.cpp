#include <iostream>
#include <asio.hpp>
#include <thread>
#include <filesystem>
#include "PrepareAnswer.hpp"

class HttpServer {
public:
    HttpServer(asio::io_context& io_context, int port = 23457);

    void start();

private:
    void doAccept();

    PrepareAnswer prepareAnswer;

    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    std::string response;
    std::array<char, 32 * 1024> m_requestBuffer;
};

HttpServer::HttpServer(asio::io_context& io_context, int port)
    : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      socket_(io_context) {}

void HttpServer::start() {
    doAccept();
}

void HttpServer::doAccept() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
        if (!ec) {

            socket_.async_read_some(asio::buffer(m_requestBuffer),
                [this](std::error_code errorCode, size_t transferredBytesCount)
                    {
                        // auto stuct = M_Uri::ParseRequest(std::string(m_requestBuffer.begin(), m_requestBuffer.begin() + transferredBytesCount));
                        // std::cout << stuct.hasQuery << std::endl;

                        response = prepareAnswer.exe(std::string(m_requestBuffer.begin(), m_requestBuffer.begin() + transferredBytesCount))->res;

                        // Asynchronous write response to the client
                        asio::async_write(socket_, asio::buffer(response), [this](std::error_code writeEc, std::size_t /*bytesWritten*/) {
                            if (!writeEc) {
                                // Close the socket after writing
                                socket_.close();

                                // Continue accepting more connections
                            }
                            doAccept(); 
                        });
                    });
        }
    });
}

int main() {

    if (!std::filesystem::exists("./front"))
    {
        std::filesystem::create_directories("./front");
    }
    std::filesystem::current_path("./front");

    try {
        asio::io_context io_context;
        HttpServer server(io_context);
        std::thread m_thread([&]()
        {
            server.start();
            io_context.run();
        });
        m_thread.join();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}