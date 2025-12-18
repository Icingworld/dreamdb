#include "dreamdb/server/session.h"

#include <iostream>

namespace dreamdb
{

Session::Session(asio::ip::tcp::socket socket, RequestHandler handler)
    : socket_(std::move(socket))
    , request_handler_(std::move(handler))
    , serializer_()
    , recv_buffer_()
    , temp_buffer_()
{
}

void Session::start()
{
    do_read();
}

void Session::do_read()
{
    auto self = shared_from_this();

    socket_.async_read_some(
        asio::buffer(temp_buffer_),
        [this, self](std::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                // 将读到的数据追加到接收缓冲区
                recv_buffer_.insert(
                    recv_buffer_.end(),
                    temp_buffer_.begin(),
                    temp_buffer_.begin() + bytes_transferred
                );

                // 尝试处理缓冲区中的完整数据包
                process_buffer();

                // 继续读取
                do_read();
            } else {
                if (ec == asio::error::eof) {
                    std::cout << "[Session] Client disconnected" << std::endl;
                } else {
                    std::cerr << "[Session] Read error: " << ec.message() << std::endl;
                }
                // 连接关闭，Session 会被销毁
            }
        }
    );
}

void Session::process_buffer()
{
    std::vector<std::uint8_t> body;

    // 循环处理缓冲区中的所有完整数据包
    while (Codec::try_decode_one(recv_buffer_, body)) {
        try {
            // 反序列化请求
            Request request = serializer_.deserialize_request(body);

            std::cout << "[Session] Received request #" << request.request_id()
                      << ": " << request.sql() << std::endl;

            // 调用请求处理器
            Response response;
            response.set_request_id(request.request_id());
            response.set_version(request.version());

            if (request_handler_) {
                auto [success, result] = request_handler_(request.sql());
                response.set_success(success);
                if (success) {
                    response.set_result(result);
                } else {
                    response.set_error_message(result);
                }
            } else {
                // 没有设置处理器，返回错误
                response.set_success(false);
                response.set_error_message("No request handler configured");
            }

            // 发送响应
            send_response(response);

        } catch (const std::exception & e) {
            std::cerr << "[Session] Error processing request: " << e.what() << std::endl;

            // 发送错误响应
            Response error_response;
            error_response.set_request_id(0);
            error_response.set_success(false);
            error_response.set_error_message(std::string("Internal error: ") + e.what());
            send_response(error_response);
        }
    }
}

void Session::send_response(const Response & response)
{
    try {
        // 序列化响应
        auto serialized = serializer_.serialize_response(response);

        // 编码（加协议头）
        auto encoded = Codec::encode(serialized);

        std::cout << "[Session] Sending response #" << response.request_id()
                  << " (success=" << response.success() << ")" << std::endl;

        // 同步发送（简单起见，生产环境应该用 async_write）
        asio::write(socket_, asio::buffer(encoded));

    } catch (const std::exception & e) {
        std::cerr << "[Session] Error sending response: " << e.what() << std::endl;
    }
}

} // namespace dreamdb

