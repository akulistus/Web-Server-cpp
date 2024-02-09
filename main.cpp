#include <thread>
#include <filesystem>
#include "HttpServer.hpp"

int main() {
    if (!std::filesystem::exists("./front")) {
        std::filesystem::create_directories("./front");
    }
    std::filesystem::current_path("./front");

    try {
        asio::io_context io_context;
        HttpServer server(io_context);

        // Number of threads you want to run
        const int num_threads = 4;
        std::vector<std::thread> threads;

        // Create threads and run io_context in each
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        // Start the server
        server.start();

        // Join threads
        for (auto& thread : threads) {
            thread.join();
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
