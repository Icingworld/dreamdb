#include "dreamdb/executor/command_executor.h"

namespace dreamdb::executor
{

CommandExecutor::CommandExecutor()
{
}

ExecutionResult CommandExecutor::execute(binder::bound::BoundStatement & bound_statement)
{
    return ExecutionResult();
}

} // namespace dreamdb::executor