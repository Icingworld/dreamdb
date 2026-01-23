#include "dreamdb/binder/bound/debug/debug_printer.h"

#include <iostream>

namespace dreamdb::binder::bound
{

std::string BoundDebugPrinter::format(const BoundStatement & statement)
{
    return statement_formatter_.format(statement);
}

void BoundDebugPrinter::print(const BoundStatement & statement)
{
    std::cout << statement_formatter_.format(statement) << std::endl;
}

} // namespace dreamdb::binder::bound
