#include "dreamdb/common/mutation_result.h"

namespace dreamdb
{

MutationResult MutationResult::make_success(std::size_t affected_count)
{
    MutationResult result;
    result.set_success(true);
    result.set_affected_count(affected_count);
    return result;
}

MutationResult MutationResult::make_failure(const std::string & error_message)
{
    MutationResult result;
    result.set_success(false);
    result.set_error_message(error_message);
    return result;
}

void MutationResult::set_success(bool success) noexcept
{
    success_ = success;
}

void MutationResult::set_error_message(const std::string & message)
{
    error_message_ = message;
}

void MutationResult::set_affected_count(std::size_t count) noexcept
{
    affected_count_ = count;
}

bool MutationResult::is_success() const noexcept
{
    return success_;
}

const std::string & MutationResult::get_error_message() const noexcept
{
    return error_message_;
}

std::size_t MutationResult::get_affected_count() const noexcept
{
    return affected_count_;
}

} // namespace dreamdb
