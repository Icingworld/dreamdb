#include "dreamdb/server/server.h"

#include <iostream>

#include "dreamdb/server/session.h"

namespace dreamdb
{

Server::Server(std::uint16_t port)
    : io_context_()
    , acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    , request_handler_(nullptr)
{
    std::cout << "[Server] Listening on port " << port << std::endl;
}

Server::~Server()
{
    stop();
}

void Server::set_request_handler(RequestHandler handler)
{
    request_handler_ = std::move(handler);
}

void Server::run()
{
    do_accept();
    io_context_.run();
}

void Server::stop()
{
    io_context_.stop();
}

void Server::do_accept()
{
    acceptor_.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "[Server] New connection from "
                          << socket.remote_endpoint().address().to_string()
                          << ":" << socket.remote_endpoint().port()
                          << std::endl;

                // 创建 Session 并启动
                auto session = std::make_shared<Session>(
                    std::move(socket),
                    request_handler_
                );
                session->start();
            } else {
                std::cerr << "[Server] Accept error: " << ec.message() << std::endl;
            }

            // 继续接受下一个连接
            do_accept();
        }
    );
}

} // namespace dreamdb

