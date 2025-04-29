#include <gtest/gtest.h>

#include "hook.h"
#include "utils/string_utils.h"

namespace hbqj {
    static const char *dll_name = "namazu.dll";

    TEST(HookTest, ListAllModules) {
        Hook hook;

        const auto &ms = hook.GetLoadedModules("ffxiv_dx11.exe");
        if (ms) {
            for (const auto &m: ms.value()) {
                hook.log.info("{}", utf16_to_utf8(std::filesystem::path(m).wstring()));
            }
        }
    }

    TEST(HookTest, LoadLibrary) {
        // GTEST_SKIP();

        Hook hook;

        const auto &dir = std::filesystem::current_path();
        // Note: Path is relative to build directory. Adjust if build location changes.
        const auto &dll_path =
                dir.parent_path().parent_path().parent_path() / "x64" / "Release" / dll_name;
        hook.log.info("current dir: {}", dir.string());
        hook.log.info("dll path: {}", utf16_to_utf8(dll_path.wstring()));


        if (!std::filesystem::exists(dll_path)) {
            hook.log.error("dll not found at {}", utf16_to_utf8(dll_path.wstring()));
        }

        auto r = LoadLibraryW(dll_path.wstring().data());

        if (r == nullptr) {
            hook.log.error("LoadLibrary failed with Error Code: {}", GetLastError());
        } else {
            hook.log.info("LoadLibrary successfully.");
        }
    }

    TEST(HookTest, InjectExample) {
        // GTEST_SKIP();
        Hook hook;

        const auto &dir = std::filesystem::current_path();
        // Note: Path is relative to build directory. Adjust if build location changes.
        const auto &dll_path =
                dir.parent_path().parent_path().parent_path() / "x64" / "Release" / dll_name;
        hook.log.info("current dir: {}", dir.string());
        hook.log.info("dll path: {}", utf16_to_utf8(dll_path.wstring()));

        const auto &result = hook.Inject(dll_path.wstring(), "ffxiv_dx11.exe");
        if (!result) {
            hook.log.error("{}", result.error());
        }
    }

    TEST(HookTest, UnloadExample) {
        // GTEST_SKIP();
        Hook hook;

        auto result = hook.Unload(L"namazu.dll", "ffxiv_dx11.exe");
        if (!result) {
            hook.log.error("{}", result.error());
        }
    }
}