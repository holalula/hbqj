#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <expected>
#include <Windows.h>
#include <TlHelp32.h>

#include "process.h"

namespace hbqj {
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
		std::expected<void, WinAPIErrorCode> Initialize(Process process);

		std::expected<Address, WinAPIErrorCode> FindSignature(std::span<const Byte> signature, std::string_view mask);

		std::expected<Address, WinAPIErrorCode> CalculateTargetOffsetCall(Address offset);

		std::expected<Address, WinAPIErrorCode> CalculateTargetOffsetMov(Address offset);

		template<size_t N>
		inline static std::span<const Byte> MakePattern(const char(&str)[N]) {
			return std::span<const Byte>(reinterpret_cast<const Byte*>(str), N - 1);
		}

		Process process_;
	private:
		bool CompareMemory(std::span<const Byte> data, std::span<const Byte> pattern, std::string_view mask);

		inline uint32_t ConvertOffset(const uint32_t* bytes) {
			return *reinterpret_cast<const uint32_t*>(bytes);
		}
	};
}