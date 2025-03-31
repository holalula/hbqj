#include <gtest/gtest.h>

#include "hook.h"

namespace hbqj {
	TEST(HookTest, Example) {
		// GTEST_SKIP();
		Hook hook;
		const auto& dir = std::filesystem::current_path();
		const auto& dll_path = dir.parent_path().parent_path().parent_path().parent_path().parent_path() / "x64" / "Release" / "bgpop.dll";
		hook.log.info("current dir: {}", dir.string());
		hook.log.info("dll path: {}", dll_path.string());

		bool inject = true;

		if (inject) {
			const auto& result = hook.Inject(dll_path.wstring(), "ffxiv_dx11.exe");
			if (!result) {
				hook.log.error("{}", result.error());
			}

		}
		else {
			auto result = hook.Unload(L"bgpop.dll", "ffxiv_dx11.exe");
			if (!result) {
				hook.log.error("{}", result.error());
			}
		}

		const auto& ms = hook.GetLoadedModules("ffxiv_dx11.exe");
		if (ms) {
			for (const auto& m : ms.value()) {
				hook.log.info("{}", std::filesystem::path(m).string());
			}
		}
	}
}