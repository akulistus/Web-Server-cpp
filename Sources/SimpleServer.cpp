// #include "SimpleServer.hpp"

// SimpleServer::SimpleServer(asio::io_context& ioContext, int port) : ioContext(ioContext), acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), socket(ioContext)
// {
// }

// void SimpleServer::handleRead(std::shared_ptr<asio::ip::tcp::socket> socket, const std::error_code& error, std::vector<char>& buffer, std::size_t bytes_transferred) {
//     if (!error) {
//         Response* rsp = prepareAnswer.exe({buffer.data(), bytes_transferred});
//         asio::async_write(*socket, asio::buffer(rsp->res.data(), rsp->res.size()),
//                           [this, socket](const std::error_code& write_error, std::size_t) {
//                               handleWrite(socket, write_error);
//                           });
//     }
// }

// void SimpleServer::handleRead(std::shared_ptr<asio::ip::tcp::socket> socket, const std::error_code& error, std::size_t bytes_transferred) {
//     if (!error) {
//         Response* rsp = prepareAnswer.exe({socket->data(), bytes_transferred});
//         asio::async_write(*socket, asio::buffer(rsp->res.data(), rsp->res.size()),
//                           [this, socket](const std::error_code& write_error, std::size_t) {
//                               handleWrite(socket, write_error, 0);
//                           });
//     }
// }

// void SimpleServer::handleWrite(std::shared_ptr<asio::ip::tcp::socket> socket, const std::error_code& error, std::size_t) {
//     if (!error) {
//         socket->close();
//         std::cout << "Connection closed." << std::endl;
//     }
// }
