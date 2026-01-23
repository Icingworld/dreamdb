#pragma once

#include <string>

#include "dreamdb/binder/bound/debug/statement_formatter.h"

namespace dreamdb::binder::bound
{

class BoundStatement;

/**
 * @brief 绑定调试打印器
 */
class BoundDebugPrinter
{
public:
    BoundDebugPrinter() noexcept = default;

    ~BoundDebugPrinter() noexcept = default;

public:
    /**
     * @brief 格式化语句
     * @param statement 语句
     * @return 格式化后的语句
     */
    std::string format(const BoundStatement & statement);

    /**
     * @brief 打印语句
     * @param statement 语句
     */
    void print(const BoundStatement & statement);

private:
    BoundStatementFormatter statement_formatter_;
};

} // namespace dreamdb::binder::bound
