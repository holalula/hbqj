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

	// disable struct padding
#pragma pack(push, 1)
	struct CallInstruction {
		Byte opcode;
		int32_t offset;
	};

	struct MovInstruction {
		Byte rex;
		Byte opcode[2];
		uint32_t offset;
	};
#pragma pack(pop)

	class __declspec(dllexport) SignatureScanner {
	public:
		std::expected<ProcessModule, WinAPIErrorCode> get_process_module(std::string_view process_name, std::string_view module_name);

		std::expected<Address, WinAPIErrorCode> find_signature(std::span<const Byte> signature, std::span<const Byte> mask);

		std::expected<Address, WinAPIErrorCode> calculate_target_offset_call(Address offset);

		std::expected<Address, WinAPIErrorCode> calculate_target_offset_mov(Address offset);

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

		inline uint32_t convert_offset(const uint32_t* bytes) {
			return *reinterpret_cast<const uint32_t*>(bytes);
		}

		ProcessModule target_module_;
		HANDLE target_process_;
		SIZE_T target_process_id_;
	};
}