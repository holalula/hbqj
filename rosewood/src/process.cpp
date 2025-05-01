#include "game_process.h"
#include "utils/string_utils.h"

namespace hbqj {
    // This should not mutate target_module_
    // TODO: Reduce the number of searches for process
    std::expected<bool, Error> Process::HasModule(std::string_view process_name, std::string_view module_name) {
        auto handle = GetProcess(process_name);
        if (!handle.has_value()) {
            return std::unexpected(handle.error());
        }

        auto hmodule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, target_process_id_);
        if (hmodule == INVALID_HANDLE_VALUE) {
            return std::unexpected(WinAPIError{.error = GetLastError()});
        }

        MODULEENTRY32W mEntry{.dwSize = sizeof(MODULEENTRY32W)};

        do {
            if (module_name == utf16_to_utf8(mEntry.szModule)) {
                CloseHandle(hmodule);
                return true;
            }
        } while (Module32NextW(hmodule, &mEntry));

        CloseHandle(hmodule);
        return false;
    }

    std::expected<ProcessModule, Error>
    Process::GetProcessModule(std::string_view process_name, std::string_view module_name) {
        auto handle = GetProcess(process_name);
        if (!handle.has_value()) {
            return std::unexpected(handle.error());
        }
        target_process_ = handle.value();

        auto module = GetModule(module_name);
        if (!module.has_value()) {
            return std::unexpected(module.error());
        }
        target_module_ = module.value();

        return target_module_;
    }

    std::expected<HANDLE, Error> Process::GetProcess(std::string_view process_name) {
        auto handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (handle == INVALID_HANDLE_VALUE) {
            return std::unexpected(WinAPIError{.error = GetLastError()});
        }

        PROCESSENTRY32W entry{.dwSize = sizeof(PROCESSENTRY32W)};

        do {
            if (process_name == utf16_to_utf8(entry.szExeFile)) {
                target_process_id_ = entry.th32ProcessID;
                CloseHandle(handle);
                target_process_ = OpenProcess(PROCESS_ALL_ACCESS, true, target_process_id_);
                return target_process_;
            }
        } while (Process32NextW(handle, &entry));

        CloseHandle(handle);
        // log.error("Process [{}] was not found.", process_name);
        return std::unexpected(WinAPIError{.error = GetLastError()});
    }

    std::expected<ProcessModule, Error> Process::GetModule(std::string_view module_name) {
        auto hmodule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, target_process_id_);
        if (hmodule == INVALID_HANDLE_VALUE) {
            return std::unexpected(WinAPIError{.error = GetLastError()});
        }

        MODULEENTRY32W mEntry{.dwSize = sizeof(MODULEENTRY32W)};

        do {
            if (module_name == utf16_to_utf8(mEntry.szModule)) {
                CloseHandle(hmodule);
                target_module_ = {.base = reinterpret_cast<Address>(mEntry.modBaseAddr), .size = mEntry.modBaseSize};
                wcscpy_s(target_module_.path, MAX_PATH, mEntry.szExePath);
                return target_module_;
            }
        } while (Module32NextW(hmodule, &mEntry));

        CloseHandle(hmodule);
        return std::unexpected(WinAPIError{.error = GetLastError()});
    }

    std::expected<Address, Error> Process::CalculateTargetOffsetCall(Address offset) {
        // call instruction (near relative)
        // E8 [32-bit offset]
        // e.g. E8 12 34 56 78; call target (target = next_instruction + 0x78563412)
        auto base_addr = target_module_.base;
        const auto &read_result = ReadMemory<CallInstruction>(base_addr + offset);
        if (!read_result) {
            return std::unexpected(read_result.error());
        }

        const auto &inst = read_result.value();

        // target = next inst addr + offset
        return offset + sizeof(CallInstruction) + inst.offset;
    }

    std::expected<Address, Error> Process::CalculateTargetOffsetMov(Address offset) {
        // mov instruction (RIP relative)
        // 48 8B 0D [32-bit offset]; mov rcx, [rip + offset]
        // 48 8B 0D 12 34 56 78; mov rcx, [rip + 0x78563412]
        auto base_addr = target_module_.base;
        const auto &read_result = ReadMemory<MovInstruction>(base_addr + offset);
        if (!read_result) {
            return std::unexpected(read_result.error());
        }

        const auto &inst = read_result.value();

        // target = next inst addr + offset
        return offset + sizeof(MovInstruction) + ConvertOffset(&inst.offset);
    }
}