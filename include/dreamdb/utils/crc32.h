#pragma once

#include <cstdint>

namespace dreamdb
{

/**
 * @brief CRC32 校验
 */
class CRC32
{
public:
    CRC32() = default;

    CRC32(const CRC32 &) = default;

    CRC32(CRC32 &&) noexcept = default;

    CRC32 & operator=(const CRC32 &) = default;

    CRC32 & operator=(CRC32 &&) noexcept = default;

    ~CRC32() = default;

public:
    /**
     * @brief 计算指定数据的 CRC32 校验和
     * @param data 数据指针
     * @param size 数据长度
     * @return CRC32 校验和
     */
    static std::uint32_t compute(const std::uint8_t * data, std::size_t size) noexcept;

    /**
     * @brief 计算指定数据的 CRC32 校验和
     * @param data 数据缓冲区
     * @param size 数据长度
     * @return CRC32 校验和
     */
    static std::uint32_t compute(const void * data, std::size_t size) noexcept;

private:
    /**
     * @brief 获取 CRC32 查找表
     * @return 指向 256 项 CRC32 查找表的指针
     */
    static const std::uint32_t * table() noexcept;

public:
    static constexpr std::uint32_t polynomial = 0xEDB88320u;    // CRC32 多项式（反射形式）
};

} // namespace dreamdb
