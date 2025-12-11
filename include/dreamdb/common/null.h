#pragma once

namespace dreamdb
{

/**
 * @brief 空值类型
*/
class Null
{
public:
    Null() = default;

    ~Null() = default;

public:
    bool operator==(const Null & other) const noexcept;

    bool operator!=(const Null & other) const noexcept;
};

} // namespace dreamdb
