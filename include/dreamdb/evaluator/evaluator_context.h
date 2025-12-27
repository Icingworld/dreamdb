#pragma once

#include "dreamdb/schema/entity.h"
#include "dreamdb/schema/collection.h"

namespace dreamdb
{

/**
 * @brief 表达式评估上下文
 * @details 提供评估表达式所需的环境信息
 */
class EvaluatorContext
{
public:
    explicit EvaluatorContext() noexcept;

    EvaluatorContext(const EvaluatorContext &) = default;

    EvaluatorContext(EvaluatorContext &&) noexcept = default;

    EvaluatorContext & operator=(const EvaluatorContext &) = default;

    EvaluatorContext & operator=(EvaluatorContext &&) noexcept = default;

    ~EvaluatorContext() = default;

public:
    /**
     * @brief 设置当前实体
     * @param entity 实体
     */
    void set_entity(const Entity * entity) noexcept;

    /**
     * @brief 设置集合 Schema
     * @param collection 集合
     */
    void set_collection(const Collection * collection) noexcept;

    /**
     * @brief 获取当前实体
     * @return 实体指针，如果未设置返回 nullptr
     */
    const Entity * get_entity() const noexcept;

    /**
     * @brief 获取集合
     * @return 集合指针，如果未设置返回 nullptr
     */
    const Collection * get_collection() const noexcept;

    /**
     * @brief 检查上下文是否有效，实体和集合都已设置时才有效
     * @return 如果有效返回 true
     */
    bool is_valid() const noexcept;

private:
    const Entity * entity_;              // 当前实体
    const Collection * collection_;      // 集合
};

} // namespace dreamdb
