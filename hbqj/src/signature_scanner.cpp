#include "signature_scanner.h"
#include "utils/string_utils.h"

namespace hbqj {
	std::expected<Address, WinAPIErrorCode> SignatureScanner::FindSignature(std::span<const Byte> signature, std::string_view mask) {
		Address addr = target_module_.base;
		Address size = target_module_.size;
		std::vector<Byte> buffer(size);
		SIZE_T bytesRead;

		if (!ReadProcessMemory(target_process_, reinterpret_cast<LPCVOID>(addr),
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

	std::expected<ProcessModule, WinAPIErrorCode> SignatureScanner::GetProcessModule(std::string_view process_name, std::string_view module_name) {
		auto handle = this->GetProcess(process_name);
		if (!handle.has_value()) {
			return std::unexpected(handle.error());
		}
		this->target_process_ = handle.value();

		auto module = this->GetModule(module_name);
		if (!module.has_value()) {
			return std::unexpected(module.error());
		}
		this->target_module_ = module.value();

		return this->target_module_;
	}

	std::expected<HANDLE, WinAPIErrorCode> SignatureScanner::GetProcess(std::string_view process_name) {
		auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (handle == INVALID_HANDLE_VALUE) {
			return std::unexpected(GetLastError());
		}

		PROCESSENTRY32W entry{ .dwSize = sizeof(PROCESSENTRY32W) };

		do {
			if (process_name == utf16_to_utf8(entry.szExeFile)) {
				this->target_process_id_ = entry.th32ProcessID;
				CloseHandle(handle);
				this->target_process_ = OpenProcess(PROCESS_ALL_ACCESS, false, this->target_process_id_);
				return this->target_process_;
			}
		} while (Process32NextW(handle, &entry));

		CloseHandle(handle);
		return std::unexpected(GetLastError());
	}

	std::expected<ProcessModule, WinAPIErrorCode> SignatureScanner::GetModule(std::string_view module_name) {
		auto hmodule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->target_process_id_);
		if (hmodule == INVALID_HANDLE_VALUE) {
			return std::unexpected(GetLastError());
		}

		MODULEENTRY32W mEntry{ .dwSize = sizeof(MODULEENTRY32W) };

		do {
			if (module_name == utf16_to_utf8(mEntry.szModule)) {
				CloseHandle(hmodule);
				this->target_module_ = { .base = reinterpret_cast<Address>(mEntry.modBaseAddr), .size = mEntry.modBaseSize };
				return this->target_module_;
			}
		} while (Module32NextW(hmodule, &mEntry));

		CloseHandle(hmodule);
		return std::unexpected(GetLastError());
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
		auto base_addr = target_module_.base;
		const auto& read_result = ReadMemory<CallInstruction>(base_addr + offset);
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
		auto base_addr = target_module_.base;
		const auto& read_result = ReadMemory<MovInstruction>(base_addr + offset);
		if (!read_result) {
			return std::unexpected(read_result.error());
		}

		const auto& inst = read_result.value();

		// target = next inst addr + offset
		return offset + sizeof(MovInstruction) + ConvertOffset(&inst.offset);
	}
}