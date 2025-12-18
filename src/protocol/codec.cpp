#include "dreamdb/protocol/codec.h"

#include <stdexcept>
#include <cstring>

#include "dreamdb/protocol/header.h"
#include "dreamdb/utils/crc32.h"

namespace dreamdb
{

std::vector<std::uint8_t> Codec::encode(const std::vector<std::uint8_t> & data)
{
    // data 为序列化后的数据

    // 1. 构造协议头
    Header header;
    header.magic_number = 0x647265616D646200ULL;
    header.total_length = static_cast<std::uint32_t>(sizeof(Header) + data.size()); // 总长度 = 头部 + 数据
    header.checksum = CRC32::compute(data.data(), data.size());          // 计算 data 的校验和

    // 2. 拼接头部和数据
    std::vector<std::uint8_t> encoded_data;
    encoded_data.reserve(sizeof(Header) + data.size());

    encoded_data.insert(
        encoded_data.end(),
        reinterpret_cast<const std::uint8_t *>(&header),
        reinterpret_cast<const std::uint8_t *>(&header) + sizeof(Header)
    );

    encoded_data.insert(encoded_data.end(), data.begin(), data.end());

    return encoded_data;
}

std::vector<std::uint8_t> Codec::decode(const std::vector<std::uint8_t> & data)
{
    // 至少要能包含一个完整的 Header
    if (data.size() < sizeof(Header)) {
        throw std::runtime_error("Codec::decode: data too short for header");
    }

    // 1. 解析头部
    Header header;
    std::memcpy(&header, data.data(), sizeof(Header));

    // 2. 校验魔数
    if (header.magic_number != 0x647265616D646200ULL) {
        throw std::runtime_error("Codec::decode: invalid magic number");
    }

    // 3. 校验长度一致性
    if (header.total_length != data.size()) {
        throw std::runtime_error("Codec::decode: total_length mismatch");
    }

    // 4. 取出数据并验证 CRC32
    const auto body_size = header.total_length - static_cast<std::uint32_t>(sizeof(Header));
    const auto * body_ptr = data.data() + sizeof(Header);

    const std::uint32_t computed_checksum = CRC32::compute(body_ptr, body_size);
    if (computed_checksum != header.checksum) {
        throw std::runtime_error("Codec::decode: checksum mismatch");
    }

    // 5. 返回 body
    return std::vector<std::uint8_t>(body_ptr, body_ptr + body_size);
}

bool Codec::try_decode_one(std::vector<std::uint8_t> & buffer,
                           std::vector<std::uint8_t> & out_body)
{
    // 1. 缓冲区太小，连头都不够，直接返回 false
    if (buffer.size() < sizeof(Header)) {
        return false;
    }

    // 2. 先读出头部（不修改 buffer）
    Header header;
    std::memcpy(&header, buffer.data(), sizeof(Header));

    // 3. 基本合法性校验：魔数 + 长度范围
    if (header.magic_number != 0x647265616D646200ULL) {
        throw std::runtime_error("Codec::try_decode_one: invalid magic number");
    }

    if (header.total_length < sizeof(Header) ||
        header.total_length > kMaxPacketSize) {
        throw std::runtime_error("Codec::try_decode_one: invalid total_length");
    }

    // 4. 检查缓冲区是否已经包含了一个完整的数据包
    if (buffer.size() < header.total_length) {
        return false; // 还没收全，继续等数据
    }

    // 5. 现在可以安全地解析 body 并校验 CRC32
    const auto body_size = header.total_length - static_cast<std::uint32_t>(sizeof(Header));
    const auto * body_ptr = buffer.data() + sizeof(Header);

    const std::uint32_t computed_checksum = CRC32::compute(body_ptr, body_size);
    if (computed_checksum != header.checksum) {
        throw std::runtime_error("Codec::try_decode_one: checksum mismatch");
    }

    // 6. 输出 body
    out_body.assign(body_ptr, body_ptr + body_size);

    // 7. 从缓冲区中移除已消费的完整数据包（头 + body）
    buffer.erase(buffer.begin(), buffer.begin() + header.total_length);

    return true;
}

} // namespace dreamdb
