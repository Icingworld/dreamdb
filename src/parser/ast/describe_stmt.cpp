#include "dreamdb/parser/ast/describe_stmt.h"

#include <sstream>

namespace dreamdb
{

DescribeStmt::DescribeStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::DESCRIBE_STMT, line, column)
    , collection_name_("")
{
}

void DescribeStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

const std::string & DescribeStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

std::string DescribeStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "DescribeStmt(";
    oss << "collection_name=" << (collection_name_.empty() ? "<none>" : collection_name_);
    oss << ")";
    return oss.str();
}

} // namespace dreamdb
