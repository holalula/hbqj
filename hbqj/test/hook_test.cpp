#include <gtest/gtest.h>

#include "hook.h"

namespace hbqj {
	TEST(HookTest, Example) {
		GTEST_SKIP();
		Hook hook;
		const auto& dir = std::filesystem::current_path();
		const auto& dll_path = dir.parent_path().parent_path().parent_path().parent_path().parent_path() / "x64" / "Release" / "bgpop.dll";
		hook.log.info("current dir: {}", dir.string());
		hook.log.info("dll path: {}", dll_path.string());
		//		const auto& result = hook.Inject(dll_path.wstring());
		//		if (!result) {
		//			hook.log.error("{}", result.error());
		//		}

		//		auto result = hook.Unload(L"bgpop.dll");
		//		if (!result) {
		//			hook.log.error("{}", result.error());
		//		}

		const auto& ms = hook.GetLoadedModules();
		if (ms) {
			for (const auto& m : ms.value()) {
				hook.log.info("{}", std::filesystem::path(m).string());
			}
		}
	}
}