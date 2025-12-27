#include "dreamdb/evaluator/evaluator_context.h"

namespace dreamdb
{

EvaluatorContext::EvaluatorContext() noexcept
    : entity_(nullptr)
    , collection_(nullptr)
{
}

void EvaluatorContext::set_entity(const Entity * entity) noexcept
{
    entity_ = entity;
}

void EvaluatorContext::set_collection(const Collection * collection) noexcept
{
    collection_ = collection;
}

const Entity * EvaluatorContext::get_entity() const noexcept
{
    return entity_;
}

const Collection * EvaluatorContext::get_collection() const noexcept
{
    return collection_;
}

bool EvaluatorContext::is_valid() const noexcept
{
    return entity_ != nullptr && collection_ != nullptr;
}

} // namespace dreamdb
