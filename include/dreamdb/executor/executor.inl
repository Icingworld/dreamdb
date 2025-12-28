#pragma once

#include "dreamdb/executor/executor.h"

namespace dreamdb
{

template<typename Func>
ExecutorResult Executor::execute_with_context(Func && func)
{
    // 现在直接执行操作
    // 未来可以在这里包装事务、日志等横切关注点：
    // 
    // auto* txn = begin_transaction();
    // try {
    //     auto result = func();
    //     txn->commit();
    //     log_operation(...);
    //     return result;
    // } catch (...) {
    //     txn->rollback();
    //     log_error(...);
    //     throw;
    // }
    
    return func();
}

} // namespace dreamdb
