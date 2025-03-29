#include "signature_scanner.h"
#include "utils/string_utils.h"

namespace hbqj {
	std::expected<void, Error> SignatureScanner::Initialize(std::shared_ptr<Process> process) {
		process_ = process;
		return {};
	}

	std::expected<Address, Error> SignatureScanner::FindSignature(std::span<const Byte> signature, std::string_view mask) {
		Address addr = process_->target_module_.base;
		Address size = process_->target_module_.size;
		std::vector<Byte> buffer(size);
		SIZE_T bytesRead;

		if (!ReadProcessMemory(process_->target_process_, reinterpret_cast<LPCVOID>(addr),
			buffer.data(), size, &bytesRead)) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		for (size_t i = 0; i < bytesRead - signature.size(); ++i) {
			if (CompareMemory(std::span(buffer).subspan(i), signature, mask)) {
				return addr + i;
			}
		}
		return std::unexpected(WinAPIError{ .error = ERROR_NOT_FOUND });
	}

	bool SignatureScanner::CompareMemory(std::span<const Byte> data, std::span<const Byte> pattern, std::string_view mask) {
		if (data.size() < pattern.size()) {
			return false;
		}
		if (pattern.size() != mask.size()) {
			return false;
		}

		for (size_t i = 0; i < mask.size(); i++) {
			if (mask[i] == 'x' && data[i] != pattern[i]) {
				return false;
			}
		}

		return true;
	}
}