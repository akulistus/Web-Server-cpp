// #include <iostream>
// #include <asio.hpp>
// #include <memory>
// #include <filesystem>
// #include "PrepareAnswer.hpp"


// class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
// public:
//     HttpConnection(asio::ip::tcp::socket socket, PrepareAnswer& prepareAnswer);

//     void start();

// private:
//     void doRead();
//     void doWrite();

//     asio::ip::tcp::socket socket_;
//     PrepareAnswer& prepareAnswer;
//     std::string response;
//     std::array<char, 32 * 1024> m_requestBuffer;
// };


// class HttpServer {
// public:
//     HttpServer(asio::io_context& io_context, int port = 23457);

//     void start();

// private:
//     void doAccept();

//     PrepareAnswer prepareAnswer;

//     asio::ip::tcp::acceptor acceptor_;
//     asio::ip::tcp::socket socket_;
//     std::string response;
//     std::array<char, 32 * 1024> m_requestBuffer;
// };


