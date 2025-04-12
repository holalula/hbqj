#include <future>

#include "signature_scanner.h"
#include "utils/string_utils.h"
#include "utils/scoped_timer.h"

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

        auto result = CompareMemorySequential(buffer, bytesRead, signature, mask);
        if (result) {
            return addr + result.value();
        }

		return std::unexpected(WinAPIError{ .error = ERROR_NOT_FOUND });
	}

    std::optional<size_t> SignatureScanner::CompareMemorySequential(
            const std::vector<Byte>& buffer,
            size_t buffer_size,
            std::span<const Byte> signature,
            std::string_view mask) {
        for (size_t i = 0; i < buffer_size - signature.size(); ++i) {
            if (SignatureScanner::CompareMemory(std::span(buffer).subspan(i), signature, mask)) {
                return i;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t> SignatureScanner::CompareMemoryParallel(
            const std::vector<Byte>& buffer,
            size_t buffer_size,
            std::span<const Byte> signature,
            std::string_view mask) {
        const size_t num_threads = std::thread::hardware_concurrency();
        const size_t chunk_size = buffer_size / num_threads;
        std::vector<std::future<std::optional<size_t>>> futures;

        const Byte* buffer_data = buffer.data();
        const size_t pattern_size = signature.size();

        for (size_t t = 0; t < num_threads; ++t) {
            size_t start = t * chunk_size;
            size_t end = (t == num_threads - 1) ? buffer_size : (t + 1) * chunk_size;

            futures.push_back(std::async(std::launch::async, [=]() {
                for (size_t i = start; i < end - pattern_size; ++i) {
                    if (SignatureScanner::CompareMemory(std::span(buffer_data + i, pattern_size), signature, mask)) {
                        return std::optional<size_t>(i);
                    }
                }
                return std::optional<size_t>();
            }));
        }

        for (auto& future : futures) {
            if (auto result = future.get()) {
                return result;
            }
        }

        return std::nullopt;
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