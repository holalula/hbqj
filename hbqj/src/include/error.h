#pragma once

#include <variant>
#include <string>

namespace hbqj {
	struct SignatureNotFoundError { std::string message; };

	using Error = std::variant<
		SignatureNotFoundError
	>;
}

