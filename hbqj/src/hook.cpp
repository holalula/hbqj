#include "hook.h"

namespace hbqj {
	std::expected<bool, Error> Hook::Inject(std::wstring_view dll_path) {
		TRY(process,
			process_.GetProcess("notepad.exe"));

		size_t path_size = (dll_path.length() + 1) * sizeof(wchar_t);
		auto path_start_addr = VirtualAllocEx(process, NULL, path_size, MEM_COMMIT, PAGE_READWRITE);
		if (!path_start_addr) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		const auto& write_result = process_.WriteMemory(reinterpret_cast<Address>(path_start_addr), dll_path.data());
		if (!write_result) {
			VirtualFreeEx(process, path_start_addr, 0, MEM_RELEASE);
			return std::unexpected(write_result.error());
		}

		const auto& kernel32 = GetModuleHandleW(L"kernel32.dll");
		if (!kernel32) {
			VirtualFreeEx(process, path_start_addr, 0, MEM_RELEASE);
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		const auto& load_lib_addr = GetProcAddress(kernel32, "LoadLibraryW");
		if (!load_lib_addr) {
			VirtualFreeEx(process, path_start_addr, 0, MEM_RELEASE);
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		const auto& thread = CreateRemoteThreadEx(process, nullptr, 0, reinterpret_cast<PTHREAD_START_ROUTINE>(load_lib_addr), path_start_addr, 0, nullptr, nullptr);
		if (!thread) {
			VirtualFreeEx(process, path_start_addr, 0, MEM_RELEASE);
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		// timeout: 10s
		const auto& wait_result = WaitForSingleObject(thread, 10000);
		if (wait_result != WAIT_OBJECT_0) {
			CloseHandle(thread);
			VirtualFreeEx(process, path_start_addr, 0, MEM_RELEASE);
			log.error("Injection timeout.");
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		DWORD exit_code = 0;
		GetExitCodeThread(thread, &exit_code);

		CloseHandle(thread);
		VirtualFreeEx(process, path_start_addr, 0, MEM_RELEASE);

		// If the function succeeds, the return value is nonzero.
		// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getexitcodethread
		if (exit_code == 0) {
			log.error("Injection failed with exit code: {}.", exit_code);
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		log.info("Injection succeeded");

		return true;
	}

	std::expected<bool, Error> Hook::Unload(std::wstring_view dll_name) {
		TRY(process,
			process_.GetProcess("notepad.exe"));

		const auto& snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(process));
		if (!snapshot) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		MODULEENTRY32W me32 = { sizeof(MODULEENTRY32W) };
		bool found = false;
		HMODULE module = nullptr;
		if (Module32FirstW(snapshot, &me32)) {
			do {
				if (_wcsicmp(me32.szModule, dll_name.data()) == 0 || _wcsicmp(me32.szExePath, dll_name.data()) == 0) {
					module = me32.hModule;
					found = true;
					break;
				}
			} while (Module32NextW(snapshot, &me32));
		}
		CloseHandle(snapshot);

		if (!found) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		const auto& kernel32 = GetModuleHandle(L"kernel32.dll");
		if (!kernel32) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		const auto& free_lib = GetProcAddress(kernel32, "FreeLibrary");
		if (!free_lib) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		const auto& thread = CreateRemoteThread(process, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(free_lib), module, 0, NULL);
		if (!thread) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		// timeout: 10s
		WaitForSingleObject(thread, 10000);

		DWORD exit_code = 0;
		GetExitCodeThread(thread, &exit_code);

		CloseHandle(thread);

		if (exit_code == 0) {
			log.error("Unload failed with exit code: {}.", exit_code);
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		return true;
	}

	std::expected<std::vector<std::wstring>, Error> Hook::GetLoadedModules() {
		TRY(process,
			process_.GetProcess("notepad.exe"));

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(process));
		if (snapshot == INVALID_HANDLE_VALUE) {
			return std::unexpected(WinAPIError{ .error = GetLastError() });
		}

		std::vector<std::wstring> modules;
		MODULEENTRY32W module_entry;
		module_entry.dwSize = sizeof(MODULEENTRY32W);

		if (Module32FirstW(snapshot, &module_entry)) {
			do {
				modules.emplace_back(module_entry.szExePath);
			} while (Module32NextW(snapshot, &module_entry));
		}

		CloseHandle(snapshot);
		return modules;
	}
}