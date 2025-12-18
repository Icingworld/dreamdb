#include "dreamdb/protocol/serializer.h"

#include <stdexcept>

namespace dreamdb
{

std::vector<uint8_t> Serializer::serialize_request(const Request & request) const
{
    const std::size_t size = request.ByteSizeLong();
    std::vector<uint8_t> data(size);

    if (!request.SerializeToArray(data.data(), static_cast<int>(size))) {
        throw std::runtime_error("Failed to serialize request");
    }

    return data;
}

std::vector<uint8_t> Serializer::serialize_response(const Response & response) const
{
    const std::size_t size = response.ByteSizeLong();
    std::vector<uint8_t> data(size);

    if (!response.SerializeToArray(data.data(), static_cast<int>(size))) {
        throw std::runtime_error("Failed to serialize response");
    }

    return data;
}

Request Serializer::deserialize_request(const std::vector<uint8_t> & data) const
{
    Request request;
    if (!request.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        throw std::runtime_error("Failed to deserialize request");
    }
    return request;
}

Response Serializer::deserialize_response(const std::vector<uint8_t> & data) const
{
    Response response;
    if (!response.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        throw std::runtime_error("Failed to deserialize response");
    }
    return response;
}

} // namespace dreamdb
