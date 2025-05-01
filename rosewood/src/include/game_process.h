#pragma once

#include <cstdint>
#include <expected>
#include <string_view>
#include <Windows.h>
#include <TlHelp32.h>

#include "error.h"
#include "log.h"

namespace hbqj {
    typedef uint64_t Address;
    typedef uint8_t Byte;

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

    struct ProcessModule {
        Address base, size;
        WCHAR path[MAX_PATH];
    };

    class __declspec(dllexport) Process {
    public:
        Process() {
            GetProcessModule("ffxiv_dx11.exe", "ffxiv_dx11.exe");
        }

        Process(std::string_view process_name, std::string_view module_name) {
            GetProcessModule(process_name, module_name);
        }

        Process(bool initialize) {
            if (initialize) {
                Process();
            }
        }

        std::expected<ProcessModule, Error>
        GetProcessModule(std::string_view process_name, std::string_view module_name);

        std::expected<bool, Error> HasModule(std::string_view process_name, std::string_view module_name);

        template<typename T>
        std::expected<bool, Error> WriteMemory(Address addr, const T &value) {
            if (!WriteProcessMemory(target_process_, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), nullptr)) {
                return std::unexpected(WinAPIError{.error = GetLastError()});
            }
            return true;
        }

        std::expected<size_t, Error> WriteMemory(Address addr, const wchar_t *str) {
            size_t len = wcslen(str) + 1;
            if (!WriteProcessMemory(target_process_, reinterpret_cast<LPVOID>(addr), str, len * sizeof(wchar_t),
                                    nullptr)) {
                return std::unexpected(WinAPIError{.error = GetLastError()});
            }
            return len * sizeof(wchar_t);
        }

        template<typename T>
        std::expected<T, Error> ReadMemory(Address addr) {
            T value{};
            if (!ReadProcessMemory(target_process_, reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), nullptr)) {
                return std::unexpected(WinAPIError{.error = GetLastError()});
            }
            return value;
        }

        constexpr Address GetBaseAddr() const {
            return target_module_.base;
        }

        constexpr Address GetOffsetAddr(const Address addr) const {
            return addr - target_module_.base;
        }

        std::expected<Address, Error> CalculateTargetOffsetCall(Address offset);

        std::expected<Address, Error> CalculateTargetOffsetMov(Address offset);

        std::expected<HANDLE, Error> GetProcess(std::string_view process_name);

        ProcessModule target_module_;

        HANDLE target_process_;

        SIZE_T target_process_id_;

        inline static Logger log = Logger::GetLogger("Process");

    private:
        std::expected<ProcessModule, Error> GetModule(std::string_view module_name);

        inline uint32_t ConvertOffset(const uint32_t *bytes) {
            return *reinterpret_cast<const uint32_t *>(bytes);
        }
    };
}