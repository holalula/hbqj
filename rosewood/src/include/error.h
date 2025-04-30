#pragma once

#include <variant>
#include <string>
#include <format>

namespace hbqj {
    struct SignatureNotFoundError {
        std::string message;
    };
    struct NullPointerError {
        std::string message;
    };
    struct WinAPIError {
        unsigned long error;
    };
    struct FileNotFoundError {
        std::string file_path;
    };

    using Error = std::variant<
            SignatureNotFoundError,
            NullPointerError,
            WinAPIError,
            FileNotFoundError
    >;
}

template<>
struct std::formatter<hbqj::Error> : std::formatter<std::string> {
    auto format(const hbqj::Error &err, format_context &ctx) const {
        std::string result;
        std::visit([&](const auto &e) {
            if constexpr (std::is_same_v<std::decay_t<decltype(e)>, hbqj::SignatureNotFoundError>) {
                result = e.message;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(e)>, hbqj::NullPointerError>) {
                result = std::format("Null Pointer at {}", e.message);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(e)>, hbqj::WinAPIError>) {
                result = std::format("WinAPI Error: {}", e.error);
            } else if constexpr (std::is_same_v<std::decay_t<decltype(e)>, hbqj::FileNotFoundError>) {
                result = std::format("FileNotFound Error: {}", e.file_path);
            }
        }, err);
        return formatter<string>::format(result, ctx);
    }
};
