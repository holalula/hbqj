#include <gtest/gtest.h>
#include <iostream>

#include "signature_scanner.h"

namespace hbqj {
	TEST(SignatureScannerTest, ScanExample) {
		GTEST_SKIP();
		SignatureScanner scanner;

		auto process = std::make_shared<Process>("ffxiv_dx11.exe", "ffxiv_dx11.exe");
		scanner.Initialize(process);

		printf("base = %llx, size = %llx\n", scanner.process_->target_module_.base, scanner.process_->target_module_.size);
		auto sig_result = scanner.FindSignature(
			// C6 83 83 01 00 00 00
			SignatureScanner::MakePattern("\xC6\x83\x83\x01\x00\x00\x00"),
			"xxxxxxx"
		);
		if (sig_result) {
			printf("signature = %llx\n", sig_result.value() - scanner.process_->target_module_.base);
		}
		else {
			printf("signature not found\n");
		}
	}

	TEST(SignatureScannerTest, CalculateOffsetExample) {
		GTEST_SKIP();
		SignatureScanner scanner;

		auto process = std::make_shared<Process>("ffxiv_dx11.exe", "ffxiv_dx11.exe");

		const auto& module_result = scanner.Initialize(process);

		if (module_result) {
			auto result = process->CalculateTargetOffsetMov(0xC53C4E);
			if (result) {
				printf("result = %llx\n", result.value());
			}
			else {
				printf("error = %d\n", result.error());
			}
		}
	}
}
