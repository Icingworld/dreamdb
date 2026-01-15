#pragma once

#include <string>

#include "dreamdb/parser/ast/debug/statement_formatter.h"

namespace dreamdb::parser::ast
{

/**
 * @brief 调试打印器
 */
class AstDebugPrinter
{
public:
    AstDebugPrinter() noexcept = default;

    ~AstDebugPrinter() noexcept = default;

public:
    /**
     * @brief 格式化语句
     * @param statement 语句
     * @return 格式化后的语句
     */
    std::string format(const AstStatement & statement);

    /**
     * @brief 打印语句
     * @param statement 语句
     */
    void print(const AstStatement & statement);

private:
    AstStatementFormatter statement_formatter_;
};

} // namespace dreamdb::parser::ast
