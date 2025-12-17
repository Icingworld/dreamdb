#pragma once

#include <cstdint>

namespace dreamdb
{

#pragma pack(push, 1)
/**
 * @brief 协议头
 */
class Header
{
public:
    Header() = default;

    Header(const Header &) = default;

    Header(Header &&) noexcept = default;

    Header & operator=(const Header &) = default;

    Header & operator=(Header &&) noexcept = default;

    ~Header() = default;

public:
    std::uint64_t magic_number;         // 8 字节魔数
    std::uint32_t total_length;         // 4 字节总长度
    std::uint32_t checksum;             // 4 字节校验和
};
#pragma pack(pop)

} // namespace dreamdb
