#pragma once
#include <cstddef>
#include <array>

// Binds the value of an expected to a variable if successful, otherwise returns the error
#define TRY(var, expr)                                    \
    auto&& _temp_##var = (expr);                              \
    if (!_temp_##var) [[unlikely]] {                          \
        return std::unexpected(_temp_##var.error());          \
    }                                                         \
    auto& var = _temp_##var.value()

