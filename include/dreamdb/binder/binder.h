#pragma once

#include <memory>
#include <string>
#include <optional>

#include <dreamdb/common/ids.h>

namespace dreamdb::catalog
{

class Catalog;

} // namespace dreamdb::catalog

namespace dreamdb::parser::ast
{

class AstStatement;
class AstSelectStatement;
class AstInsertStatement;
class AstDeleteStatement;
class AstUpdateStatement;
class AstUseStatement;
class AstShowStatement;
class AstDescribeStatement;
class AstDropStatement;
class AstCreateStatement;
class AstAlterStatement;
class AstExpression;

} // namespace dreamdb::parser

namespace dreamdb::binder
{

namespace bound
{

class BoundStatement;
class BoundExpression;

} // namespace bound

/**
 * @brief 绑定器上下文
 * @details 该上下文只是暂时用于测试 Binder，后续将替换为真正的上下文管理器
 */
struct BinderContext
{
    std::optional<std::string> current_database_name;                    // 当前数据库名称
    std::optional<dreamdb::common::database_id_t> current_database_id;   // 当前数据库 ID
};

/**
 * @brief 绑定器
 */
class Binder
{
public:
    explicit Binder(const dreamdb::catalog::Catalog & catalog);

    ~Binder() noexcept = default;

public:
    /**
     * @brief 绑定语句
     * @param statement AstStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind(
        const dreamdb::parser::ast::AstStatement & statement
    );

private:
    /**
     * @brief 绑定 ALTER 语句
     * @param alter_statement AstAlterStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_alter_statement(
        const dreamdb::parser::ast::AstAlterStatement & alter_statement
    );

    /**
     * @brief 绑定 CREATE 语句
     * @param create_statement AstCreateStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_create_statement(
        const dreamdb::parser::ast::AstCreateStatement & create_statement
    );

    /**
     * @brief 绑定 DELETE 语句
     * @param delete_statement AstDeleteStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_delete_statement(
        const dreamdb::parser::ast::AstDeleteStatement & delete_statement
    );

    /**
     * @brief 绑定 DESCRIBE 语句
     * @param describe_statement AstDescribeStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_describe_statement(
        const dreamdb::parser::ast::AstDescribeStatement & describe_statement
    );

    /**
     * @brief 绑定 DROP 语句
     * @param drop_statement AstDropStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_drop_statement(
        const dreamdb::parser::ast::AstDropStatement & drop_statement
    );

    /**
     * @brief 绑定 INSERT 语句
     * @param insert_statement AstInsertStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_insert_statement(
        const dreamdb::parser::ast::AstInsertStatement & insert_statement
    );

    /**
     * @brief 绑定 SELECT 语句
     * @param select_statement AstSelectStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_select_statement(
        const dreamdb::parser::ast::AstSelectStatement & select_statement
    );

    /**
     * @brief 绑定 SHOW 语句
     * @param show_statement AstShowStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_show_statement(
        const dreamdb::parser::ast::AstShowStatement & show_statement
    );

    /**
     * @brief 绑定 UPDATE 语句
     * @param update_statement AstUpdateStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_update_statement(
        const dreamdb::parser::ast::AstUpdateStatement & update_statement
    );

    /**
     * @brief 绑定 USE 语句
     * @param use_statement AstUseStatement 语句节点引用
     * @return 绑定后的语句指针
     */
    std::unique_ptr<bound::BoundStatement> bind_use_statement(
        const dreamdb::parser::ast::AstUseStatement & use_statement
    );

    /**
     * @brief 绑定表达式
     * @param expression AST 表达式节点
     * @return 绑定后的表达式
     */
    std::unique_ptr<bound::BoundExpression> bind_expression(
        const dreamdb::parser::ast::AstExpression & expression
    );

private:
    const dreamdb::catalog::Catalog & catalog_;   // 表结构元数据目录
    BinderContext context_;                       // 绑定器上下文
};

} // namespace dreamdb::binder
