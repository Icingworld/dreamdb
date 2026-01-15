#include "dreamdb/parser/ast/debug/debug_printer.h"

#include <iostream>

namespace dreamdb::parser::ast
{

std::string AstDebugPrinter::format(const AstStatement & statement)
{
    return statement_formatter_.format(statement);
}

void AstDebugPrinter::print(const AstStatement & statement)
{
    std::cout << statement_formatter_.format(statement) << std::endl;
}

} // namespace dreamdb::parser::ast
