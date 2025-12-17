#pragma once

#include <vector>
#include <cstdint>

#include "dreamdb/protocol/message.pb.h"

namespace dreamdb
{

/**
 * @brief 序列化器
 */
class Serializer
{
public:
    Serializer() = default;

    Serializer(const Serializer&) = default;

    Serializer(Serializer&&) = default;

    Serializer& operator=(const Serializer&) = default;

    Serializer& operator=(Serializer&&) = default;

    ~Serializer() = default;

public:
    /**
     * @brief 序列化请求
     * @param request 请求
     * @return 序列化后的请求
     */
    std::vector<uint8_t> serialize_request(const Request & request) const;

    /**
     * @brief 序列化响应
     * @param response 响应
     * @return 序列化后的响应
     */
    std::vector<uint8_t> serialize_response(const Response & response) const;

    /**
     * @brief 反序列化请求
     * @param data 数据
     * @return 反序列化后的请求
     */
    Request deserialize_request(const std::vector<uint8_t> & data) const;

    /**
     * @brief 反序列化响应
     * @param data 数据
     * @return 反序列化后的响应
     */
    Response deserialize_response(const std::vector<uint8_t> & data) const;
};

} // namespace dreamdb
