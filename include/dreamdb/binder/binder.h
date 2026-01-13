#pragma once

#include <memory>
#include <string>

#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/parser/ast/ast_statement_node.h"
#include "dreamdb/parser/ast/ast_select_statement_node.h"
#include "dreamdb/parser/ast/ast_insert_statement_node.h"
#include "dreamdb/parser/ast/ast_delete_statement_node.h"
#include "dreamdb/parser/ast/ast_update_statement_node.h"
#include "dreamdb/parser/ast/ast_use_statement_node.h"
#include "dreamdb/parser/ast/ast_show_statement_node.h"
#include "dreamdb/parser/ast/ast_describe_statement_node.h"
#include "dreamdb/parser/ast/ast_drop_statement_node.h"
#include "dreamdb/parser/ast/ast_create_statement_node.h"
#include "dreamdb/parser/ast/ast_alter_statement_node.h"
#include "dreamdb/parser/ast/ast_expression_node.h"
#include "dreamdb/expression/expression.h"
#include "dreamdb/catalog/catalog.h"

namespace dreamdb
{

class CatalogCollectionEntry;

class Binder
{
public:
    /**
     * @brief 构造函数
     * @param catalog Catalog 引用（Binder 只读取，不修改）
     * @param current_database 当前数据库名称
     */
    explicit Binder(const Catalog & catalog, const std::string & current_database);

    /**
     * @brief 绑定语句
     * @param statement AstStatementNode 语句节点引用
     * @return std::unique_ptr<BoundStatement> 绑定后的语句指针
     */
    std::unique_ptr<BoundStatement> bind(const AstStatementNode & statement);

private:
    /**
     * @brief 绑定表达式
     * @param expression AST 表达式节点
     * @return 绑定后的表达式
     */
    std::unique_ptr<Expression> bind_expression(const AstExpressionNode & expression);

    /**
     * @brief 绑定表达式（带集合上下文）
     * @param expression AST 表达式节点
     * @param collection_entry 集合条目（用于列引用绑定）
     * @return 绑定后的表达式
     */
    std::unique_ptr<Expression> bind_expression(const AstExpressionNode & expression, const CatalogCollectionEntry * collection_entry);
    
    std::unique_ptr<BoundStatement> bind_select_statement(const AstSelectStatementNode & select_statement);

    std::unique_ptr<BoundStatement> bind_insert_statement(const AstInsertStatementNode & insert_statement);

    std::unique_ptr<BoundStatement> bind_delete_statement(const AstDeleteStatementNode & delete_statement);

    std::unique_ptr<BoundStatement> bind_update_statement(const AstUpdateStatementNode & update_statement);

    std::unique_ptr<BoundStatement> bind_use_statement(const AstUseStatementNode & use_statement);

    std::unique_ptr<BoundStatement> bind_show_statement(const AstShowStatementNode & show_statement);

    std::unique_ptr<BoundStatement> bind_describe_statement(const AstDescribeStatementNode & describe_statement);

    std::unique_ptr<BoundStatement> bind_drop_statement(const AstDropStatementNode & drop_statement);

    std::unique_ptr<BoundStatement> bind_create_statement(const AstCreateStatementNode & create_statement);

    std::unique_ptr<BoundStatement> bind_alter_statement(const AstAlterStatementNode & alter_statement);

private:
    const Catalog & catalog_;                   // 表结构元数据目录（只读引用）
    std::string current_database_;              // 当前数据库
};

} // namespace dreamdb
