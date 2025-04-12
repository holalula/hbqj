#pragma once

#include <expected>
#include <iostream>
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <memory>
#include <Windows.h>
#include <TlHelp32.h>

#include "error.h"
#include "process.h"
#include "log.h"

namespace hbqj {
	class __declspec(dllexport) SignatureScanner {
	public:
		std::expected<void, Error> Initialize(std::shared_ptr<Process> process);

		std::expected<Address, Error> FindSignature(std::span<const Byte> signature, std::string_view mask);

		template<size_t N>
		inline static std::span<const Byte> MakePattern(const char(&str)[N]) {
			return std::span<const Byte>(reinterpret_cast<const Byte*>(str), N - 1);
		}

		Logger log = Logger::GetLogger("Process");
		std::shared_ptr<Process> process_;
	private:
        static bool CompareMemory(std::span<const Byte> data, std::span<const Byte> pattern, std::string_view mask);

        std::optional<size_t> CompareMemorySequential(
                const std::vector<Byte>& buffer,
                size_t buffer_size,
                std::span<const Byte> signature,
                std::string_view mask);

        std::optional<size_t> CompareMemoryParallel(
                const std::vector<Byte>& buffer,
                size_t buffer_size,
                std::span<const Byte> signature,
                std::string_view mask);
	};
}