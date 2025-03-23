#include <gtest/gtest.h>
#include <iostream>

#include "signature_scanner.h"

namespace hbqj {
	TEST(SignatureScannerTest, ScanExample) {
		GTEST_SKIP();
		SignatureScanner scanner;
		const auto& module_result = scanner.GetProcessModule("ffxiv_dx11.exe", "ffxiv_dx11.exe");

		if (module_result) {
			printf("base = %llx, size = %llx\n", module_result.value().base, module_result.value().size);
			auto sig_result = scanner.FindSignature(
				// C6 83 83 01 00 00 00
				SignatureScanner::MakePattern("\xC6\x83\x83\x01\x00\x00\x00"),
				"xxxxxxx"
			);
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
		const auto& module_result = scanner.GetProcessModule("ffxiv_dx11.exe", "ffxiv_dx11.exe");

		if (module_result) {
			auto result = scanner.CalculateTargetOffsetMov(0xC53C4E);
			if (result) {
				printf("result = %llx\n", result.value());
			}
			else {
				printf("error = %d\n", result.error());
			}
		}
	}
}
