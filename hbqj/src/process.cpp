#include "process.h"
#include "utils/string_utils.h"

namespace hbqj {
	std::expected<ProcessModule, WinAPIErrorCode> Process::GetProcessModule(std::string_view process_name, std::string_view module_name) {
		auto handle = GetProcess(process_name);
		if (!handle.has_value()) {
			return std::unexpected(handle.error());
		}
		target_process_ = handle.value();

		auto module = GetModule(module_name);
		if (!module.has_value()) {
			return std::unexpected(module.error());
		}
		target_module_ = module.value();

		return target_module_;
	}

	std::expected<HANDLE, WinAPIErrorCode> Process::GetProcess(std::string_view process_name) {
		auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (handle == INVALID_HANDLE_VALUE) {
			return std::unexpected(GetLastError());
		}

		PROCESSENTRY32W entry{ .dwSize = sizeof(PROCESSENTRY32W) };

		do {
			if (process_name == utf16_to_utf8(entry.szExeFile)) {
				target_process_id_ = entry.th32ProcessID;
				CloseHandle(handle);
				target_process_ = OpenProcess(PROCESS_ALL_ACCESS, false, target_process_id_);
				return target_process_;
			}
		} while (Process32NextW(handle, &entry));

		CloseHandle(handle);
		return std::unexpected(GetLastError());
	}

	std::expected<ProcessModule, WinAPIErrorCode> Process::GetModule(std::string_view module_name) {
		auto hmodule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, target_process_id_);
		if (hmodule == INVALID_HANDLE_VALUE) {
			return std::unexpected(GetLastError());
		}

		MODULEENTRY32W mEntry{ .dwSize = sizeof(MODULEENTRY32W) };

		do {
			if (module_name == utf16_to_utf8(mEntry.szModule)) {
				CloseHandle(hmodule);
				target_module_ = { .base = reinterpret_cast<Address>(mEntry.modBaseAddr), .size = mEntry.modBaseSize };
				return target_module_;
			}
		} while (Module32NextW(hmodule, &mEntry));

		CloseHandle(hmodule);
		return std::unexpected(GetLastError());
	}
}