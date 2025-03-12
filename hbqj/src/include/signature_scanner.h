#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <expected>
#include <Windows.h>
#include <TlHelp32.h>

#include "signature_manager.h"

namespace hbqj {
	typedef DWORD WinAPIErrorCode;
	typedef uint8_t Byte;

	struct ProcessModule {
		Address base, size;
	};

	class __declspec(dllexport) SignatureScanner {
	public:
		std::expected<ProcessModule, WinAPIErrorCode> get_process_module(std::string_view process_name, std::string_view module_name);

		std::expected<Address, WinAPIErrorCode> find_signature(std::span<const Byte> signature, std::span<const Byte> mask);

	private:
		std::expected<HANDLE, WinAPIErrorCode> get_process(std::string_view process_name);
		std::expected<ProcessModule, WinAPIErrorCode> get_module(std::string_view module_name);
		bool compare_memory(std::span<const Byte> data, std::span<const Byte> pattern, std::span<const Byte> mask);
		template <typename T>
		std::expected<bool, WinAPIErrorCode> write_memory(Address addr, const T& value) {
			if (!WriteProcessMemory(this->target_process_, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), nullptr)) {
				return std::unexpected(GetLastError());
			}
			return true;
		}
		template <typename T>
		std::expected<T, WinAPIErrorCode> read_memory(Address addr) {
			T value{};
			if (!ReadProcessMemory(this->target_process_, reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), nullptr)) {
				return std::unexpected(GetLastError());
			}

			return value;

		}

		ProcessModule target_module_;
		HANDLE target_process_;
		SIZE_T target_process_id_;
	};
}