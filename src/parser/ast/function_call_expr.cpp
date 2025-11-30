#include "dreamdb/parser/ast/function_call_expr.h"

#include <sstream>

namespace dreamdb
{

FunctionCallExpr::FunctionCallExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::FUNCTION_CALL_EXPR, line, column)
    , function_name()
    , arguments()
{
}

void FunctionCallExpr::set_function_name(const std::string & function_name) noexcept
{
    this->function_name = function_name;
}

const std::string & FunctionCallExpr::get_function_name() const noexcept
{
    return function_name;
}

void FunctionCallExpr::add_argument(std::unique_ptr<AstNode> arg)
{
    arguments.push_back(std::move(arg));
}

const std::vector<std::unique_ptr<AstNode>> & FunctionCallExpr::get_arguments() const noexcept
{
    return arguments;
}

std::size_t FunctionCallExpr::get_argument_count() const noexcept
{
    return arguments.size();
}

std::string FunctionCallExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "FunctionCallExpr(name=";
    
    if (function_name.empty()) {
        oss << "<unnamed>";
    } else {
        oss << function_name;
    }
    
    oss << ", args=[";
    for (std::size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        if (arguments[i]) {
            oss << arguments[i]->debug_string();
        } else {
            oss << "<null>";
        }
    }
    oss << "])";
    
    return oss.str();
}

} // namespace dreamdb
