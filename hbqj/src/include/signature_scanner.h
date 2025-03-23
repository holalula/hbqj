#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <expected>
#include <Windows.h>
#include <TlHelp32.h>

namespace hbqj {
	typedef unsigned __int64 Address;
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
		std::expected<ProcessModule, WinAPIErrorCode> GetProcessModule(std::string_view process_name, std::string_view module_name);

		std::expected<Address, WinAPIErrorCode> FindSignature(std::span<const Byte> signature, std::string_view mask);

		std::expected<Address, WinAPIErrorCode> CalculateTargetOffsetCall(Address offset);

		std::expected<Address, WinAPIErrorCode> CalculateTargetOffsetMov(Address offset);

		template<size_t N>
		inline static std::span<const Byte> MakePattern(const char(&str)[N]) {
			return std::span<const Byte>(reinterpret_cast<const Byte*>(str), N - 1);
		}

		inline const Address GetOffsetAddr(const Address addr) const {
			return addr - target_module_.base;
		}

	private:
		std::expected<HANDLE, WinAPIErrorCode> GetProcess(std::string_view process_name);
		std::expected<ProcessModule, WinAPIErrorCode> GetModule(std::string_view module_name);
		bool CompareMemory(std::span<const Byte> data, std::span<const Byte> pattern, std::string_view mask);
		template <typename T>
		std::expected<bool, WinAPIErrorCode> WriteMemory(Address addr, const T& value) {
			if (!WriteProcessMemory(this->target_process_, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), nullptr)) {
				return std::unexpected(GetLastError());
			}
			return true;
		}
		template <typename T>
		std::expected<T, WinAPIErrorCode> ReadMemory(Address addr) {
			T value{};
			if (!ReadProcessMemory(this->target_process_, reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), nullptr)) {
				return std::unexpected(GetLastError());
			}

			return value;

		}

		inline uint32_t ConvertOffset(const uint32_t* bytes) {
			return *reinterpret_cast<const uint32_t*>(bytes);
		}

		ProcessModule target_module_;
		HANDLE target_process_;
		SIZE_T target_process_id_;
	};
}