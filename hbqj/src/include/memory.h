#pragma once

#include "error.h"
#include "signature_manager.h"
#include "process.h"

// Binds the value of an expected to a variable if successful, otherwise returns the error
#define TRY(var, expr)                                    \
    auto&& _temp_##var = (expr);                              \
    if (!_temp_##var) [[unlikely]] {                          \
        return std::unexpected(_temp_##var.error());          \
    }                                                         \
    auto& var = *_temp_##var.value()

namespace hbqj {
	class __declspec(dllexport) Memory {
	public:
		std::expected<void, Error> Initialize(std::shared_ptr<Process> process) {
			process_ = process;
			signature_manager_.Initialize(process_);
			return {};
		}

		std::expected<void, Error> PlaceAnywhere(bool enable);

		std::shared_ptr<Process> process_;
	private:
		SignatureManager signature_manager_;
		Logger log = Logger::GetLogger("Memory");
	};
}