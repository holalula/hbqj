#include <gtest/gtest.h>
#include <iostream>

#include "signature_scanner.h"

namespace hbqj {
	TEST(SignatureScannerTest, test1) {
		SignatureScanner scanner;
		auto result = scanner.get_process_module("ffxiv_dx11.exe", "ffxiv_dx11.exe");

		if (result) {
			printf("ok: %d, size = %d\n", result.value().base, result.value().size);
		}
		else {
			printf("error = %d\n", result.error());
		}
	}
}
