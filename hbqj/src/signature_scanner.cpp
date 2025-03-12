#include "signature_scanner.h"
#include "utils/string_utils.h"

namespace hbqj {
	std::expected<Address, WinAPIErrorCode> SignatureScanner::find_signature(Address addr, SIZE_T size, std::span<const Byte> signature,
		std::span<const Byte> mask) {
		std::vector<Byte> buffer(size);
		SIZE_T bytesRead;

		if (!ReadProcessMemory(target_process_, reinterpret_cast<LPCVOID>(addr),
			buffer.data(), size, &bytesRead)) {
			return std::unexpected(GetLastError());
		}

		for (size_t i = 0; i < bytesRead - signature.size(); ++i) {
			if (compare_memory(std::span(buffer).subspan(i), signature, mask)) {
				return addr + i;
			}
		}
		return std::unexpected(ERROR_NOT_FOUND);
	}

	std::expected<ProcessModule, WinAPIErrorCode> SignatureScanner::get_process_module(std::string_view process_name, std::string_view module_name) {
		auto handle = this->get_process(process_name);
		if (!handle.has_value()) {
			return std::unexpected(handle.error());
		}
		this->target_process_ = handle.value();

		auto module = this->get_module(module_name);
		if (!module.has_value()) {
			return std::unexpected(module.error());
		}
		this->target_module_ = module.value();

		return this->target_module_;
	}

	std::expected<HANDLE, WinAPIErrorCode> SignatureScanner::get_process(std::string_view process_name) {
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

	std::expected<ProcessModule, WinAPIErrorCode> SignatureScanner::get_module(std::string_view module_name) {
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

	bool SignatureScanner::compare_memory(std::span<const Byte> data, std::span<const Byte> pattern, std::span<const Byte> mask) {
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