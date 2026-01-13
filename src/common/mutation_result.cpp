#include "dreamdb/common/mutation_result.h"

namespace dreamdb
{

MutationResult::MutationResult()
    : success_(false)
    , message_()
    , affected_count_(0)
{
}

MutationResult MutationResult::make_success()
{
    MutationResult result;
    result.set_success(true);
    return result;
}

MutationResult MutationResult::make_failure(const std::string & message)
{
    MutationResult result;
    result.set_success(false);
    result.set_message(message);
    return result;
}

void MutationResult::set_success(bool success) noexcept
{
    success_ = success;
}

void MutationResult::set_message(const std::string & message)
{
    message_ = message;
}

void MutationResult::set_affected_count(std::size_t count) noexcept
{
    affected_count_ = count;
}

bool MutationResult::is_success() const noexcept
{
    return success_;
}

const std::string & MutationResult::get_message() const noexcept
{
    return message_;
}

std::size_t MutationResult::get_affected_count() const noexcept
{
    return affected_count_;
}

} // namespace dreamdb
