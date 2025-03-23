#include "signature_scanner.h"
#include "utils/string_utils.h"

namespace hbqj {
	std::expected<void, WinAPIErrorCode> SignatureScanner::Initialize(Process process) {
		process_ = std::move(process);
		return {};
	}

	std::expected<Address, WinAPIErrorCode> SignatureScanner::FindSignature(std::span<const Byte> signature, std::string_view mask) {
		Address addr = process_.target_module_.base;
		Address size = process_.target_module_.size;
		std::vector<Byte> buffer(size);
		SIZE_T bytesRead;

		if (!ReadProcessMemory(process_.target_process_, reinterpret_cast<LPCVOID>(addr),
			buffer.data(), size, &bytesRead)) {
			return std::unexpected(GetLastError());
		}

		for (size_t i = 0; i < bytesRead - signature.size(); ++i) {
			if (CompareMemory(std::span(buffer).subspan(i), signature, mask)) {
				return addr + i;
			}
		}
		return std::unexpected(ERROR_NOT_FOUND);
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

	std::expected<Address, WinAPIErrorCode> SignatureScanner::CalculateTargetOffsetCall(Address offset) {
		// call instruction (near relative)
		// E8 [32-bit offset]
		// e.g. E8 12 34 56 78; call target (target = next_instruction + 0x78563412)
		auto base_addr = process_.target_module_.base;
		const auto& read_result = process_.ReadMemory<CallInstruction>(base_addr + offset);
		if (!read_result) {
			return std::unexpected(read_result.error());
		}

		const auto& inst = read_result.value();

		// target = next inst addr + offset
		return offset + sizeof(CallInstruction) + inst.offset;
	}

	std::expected<Address, WinAPIErrorCode> SignatureScanner::CalculateTargetOffsetMov(Address offset) {
		// mov instruction (RIP relative)
		// 48 8B 0D [32-bit offset]; mov rcx, [rip + offset]
		// 48 8B 0D 12 34 56 78; mov rcx, [rip + 0x78563412]
		auto base_addr = process_.target_module_.base;
		const auto& read_result = process_.ReadMemory<MovInstruction>(base_addr + offset);
		if (!read_result) {
			return std::unexpected(read_result.error());
		}

		const auto& inst = read_result.value();

		// target = next inst addr + offset
		return offset + sizeof(MovInstruction) + ConvertOffset(&inst.offset);
	}
}