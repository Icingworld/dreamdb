#pragma once

#include <vector>
#include <cstdint>

namespace dreamdb
{

/**
 * @brief 编解码器
 */
class Codec
{
public:
    Codec() = default;

    Codec(const Codec &) = default;

    Codec(Codec &&) noexcept = default;

    Codec & operator=(const Codec &) = default;

    Codec & operator=(Codec &&) noexcept = default;

    ~Codec() = default;

public:
    /**
     * @brief 编码
     * @param data 序列化后的数据
     * @return 编码后的数据
     */
    static std::vector<std::uint8_t> encode(const std::vector<std::uint8_t> & data);

    /**
     * @brief 解码
     * @param data 编码后的数据
     * @return 解码后的数据
     * @throw std::runtime_error 如果解码失败
     */
    static std::vector<std::uint8_t> decode(const std::vector<std::uint8_t> & data);

    /**
     * @brief 从缓冲区中尝试解码一个完整数据包
     * @details
     *  - 如果缓冲区数据不足以构成一个完整的数据包，则返回 false，buffer 不会被修改
     *  - 如果成功解析出一个完整数据包，则返回 true，并将解析出的数据写入 out_body，
     *    同时从 buffer 中移除已消费的字节（包含头部和数据）
     * @param buffer 接收缓冲区（可能包含多个数据包或半包）
     * @param out_body 输出参数，用于存放解析出的数据
     * @return 如果成功解析出一个完整数据包返回 true，否则返回 false
     * @throw std::runtime_error 如果头部非法、长度非法或校验失败
     */
    static bool try_decode_one(std::vector<std::uint8_t> & buffer,
                               std::vector<std::uint8_t> & out_body);

public:
    /**
     * @brief 允许的最大数据包大小（字节数），用于防御恶意数据
     */
    static constexpr std::uint32_t kMaxPacketSize = 64 * 1024 * 1024; // 64MB
};

} // namespace dreamdb
