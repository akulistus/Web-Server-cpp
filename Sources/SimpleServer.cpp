// #include "SimpleServer.hpp"

// HttpServer::HttpServer(asio::io_context& io_context, int port)
//     : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
//       socket_(io_context) {}

// void HttpServer::start() {
//     doAccept();
// }

// void HttpServer::doAccept() {
//     acceptor_.async_accept(
//         [this](std::error_code ec, asio::ip::tcp::socket socket)
//         {
//             if (!ec) {
//                 // Create a new connection and start handling it
//                 std::make_shared<HttpConnection>(socket, prepareAnswer)->start();
//             }

//             // Continue accepting more connections
//             doAccept();
//         });
// }

// HttpConnection::HttpConnection(asio::ip::tcp::socket socket, PrepareAnswer& prepareAnswer)
//     : socket_(std::move(socket)), prepareAnswer(prepareAnswer) {}

// void HttpConnection::start() {
//     // Start reading from the client
//     doRead();
// }

// void HttpConnection::doRead() {
//     auto self = shared_from_this();
//     socket_.async_read_some(asio::buffer(m_requestBuffer),
//         [this, self](std::error_code errorCode, size_t transferredBytesCount)
//         {
//             if (!errorCode) {
//                 // Process the request
//                 response = prepareAnswer.exe(std::string(m_requestBuffer.begin(), m_requestBuffer.begin() + transferredBytesCount))->res;

//                 // Start writing the response
//                 doWrite();
//             }
//         });
// }

// void HttpConnection::doWrite() {
//     auto self = shared_from_this();
//     asio::async_write(socket_, asio::buffer(response),
//         [this, self](std::error_code writeEc, std::size_t /*bytesWritten*/)
//         {
//             if (!writeEc) {
//                 // Close the socket after writing
//                 socket_.close();
//             }
//         });
// }
