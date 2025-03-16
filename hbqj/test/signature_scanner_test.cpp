#include <gtest/gtest.h>
#include <iostream>

#include "signature_scanner.h"

namespace hbqj {
	TEST(SignatureScannerTest, ScanExample) {
		GTEST_SKIP();
		SignatureScanner scanner;
		const auto& module_result = scanner.get_process_module("ffxiv_dx11.exe", "ffxiv_dx11.exe");

		if (module_result) {
			printf("base = %llx, size = %llx\n", module_result.value().base, module_result.value().size);
			auto sig_result = scanner.find_signature(
				// C6 83 83 01 00 00 00
				std::span<const Byte>(reinterpret_cast<const Byte*>("\xC6\x83\x83\x01\x00\x00\x00"), 7),
				std::span<const Byte>(reinterpret_cast<const Byte*>("xxxxxxx"), 7));
			if (sig_result) {
				printf("signature = %llx\n", sig_result.value() - module_result.value().base);
			}
			else {
				printf("signature not found\n");
			}
		}
		else {
			printf("error = %d\n", module_result.error());
		}
	}

	TEST(SignatureScannerTest, CalculateOffsetExample) {
		GTEST_SKIP();
		SignatureScanner scanner;
		const auto& module_result = scanner.get_process_module("ffxiv_dx11.exe", "ffxiv_dx11.exe");

		if (module_result) {
			auto result = scanner.calculate_target_offset_mov(0xC53C4E);
			if (result) {
				printf("result = %llx\n", result.value());
			}
			else {
				printf("error = %d\n", result.error());
			}
		}
	}
}
