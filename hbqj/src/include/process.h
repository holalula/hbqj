#pragma once

#include <expected>
#include <string_view>
#include <Windows.h>
#include <TlHelp32.h>

namespace hbqj {
	typedef unsigned __int64 Address;
	typedef DWORD WinAPIErrorCode;
	typedef uint8_t Byte;

	struct ProcessModule {
		Address base, size;
	};

	class __declspec(dllexport) Process {
	public:
		Process() = default;

		Process(std::string_view process_name, std::string_view module_name) {
			GetProcessModule(process_name, module_name);
		}

		std::expected<ProcessModule, WinAPIErrorCode> GetProcessModule(std::string_view process_name, std::string_view module_name);

		template <typename T>
		std::expected<bool, WinAPIErrorCode> WriteMemory(Address addr, const T& value) {
			if (!WriteProcessMemory(target_process_, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), nullptr)) {
				return std::unexpected(GetLastError());
			}
			return true;
		}
		template <typename T>
		std::expected<T, WinAPIErrorCode> ReadMemory(Address addr) {
			T value{};
			if (!ReadProcessMemory(target_process_, reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), nullptr)) {
				return std::unexpected(GetLastError());
			}

			return value;

		}

		constexpr Address GetBaseAddr() const {
			return target_module_.base;
		}

		constexpr Address GetOffsetAddr(const Address addr) const {
			return addr - target_module_.base;
		}

		ProcessModule target_module_;
		HANDLE target_process_;
		SIZE_T target_process_id_;
	private:
		std::expected<HANDLE, WinAPIErrorCode> GetProcess(std::string_view process_name);
		std::expected<ProcessModule, WinAPIErrorCode> GetModule(std::string_view module_name);
	};
}