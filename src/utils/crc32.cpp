#include "dreamdb/utils/crc32.h"

#include <array>

namespace dreamdb
{

namespace // anonymous namespace
{

// 生成单个 CRC32 表项（反射多项式）
constexpr std::uint32_t generate_entry(std::uint32_t index) noexcept
{
    std::uint32_t crc = index;
    for (int i = 0; i < 8; ++i) {
        if (crc & 1u) {
            crc = (crc >> 1) ^ CRC32::polynomial;
        } else {
            crc >>= 1;
        }
    }
    return crc;
}

// 生成完整 CRC32 查找表
constexpr std::array<std::uint32_t, 256> generate_table() noexcept
{
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t i = 0; i < 256; ++i) {
        table[i] = generate_entry(i);
    }
    return table;
}

// 编译期生成查找表
static constexpr std::array<std::uint32_t, 256> crc32_table = generate_table();

} // anonymous namespace

const std::uint32_t * CRC32::table() noexcept
{
    return crc32_table.data();
}

std::uint32_t CRC32::compute(const std::uint8_t * data, std::size_t size) noexcept
{
    const auto * tbl = table();
    std::uint32_t crc = 0xFFFFFFFFu;

    for (std::size_t i = 0; i < size; ++i) {
        std::uint8_t index = static_cast<std::uint8_t>((crc ^ data[i]) & 0xFFu);
        crc = (crc >> 8) ^ tbl[index];
    }

    return crc ^ 0xFFFFFFFFu;
}

std::uint32_t CRC32::compute(const void * data, std::size_t size) noexcept
{
    auto bytes = static_cast<const std::uint8_t *>(data);
    return compute(bytes, size);
}

} // namespace dreamdb
