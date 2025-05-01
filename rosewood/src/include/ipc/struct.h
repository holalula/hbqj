#pragma once

namespace hbqj {
#pragma pack(push, 1)
    struct SharedMemory {
        int data1;
        char event1_name[64];
        int data2;
        char event2_name[64];
        char exit_event_name[64];
        bool imguizmo_on = false;
    };
#pragma pack(pop)

    static void SafeCloseHandle(HANDLE handle) {
        // https://stackoverflow.com/questions/47575594/is-it-safe-to-call-closehandle-handle-who-handle-is-null
        if (handle && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }

    static bool IsValidHandle(HANDLE handle) {
        return handle && handle != INVALID_HANDLE_VALUE;
    }

    static void SafeLocalFree(PSECURITY_DESCRIPTOR sd) {
        if (sd) {
            LocalFree(sd);
        }
    }

    static void SafeUnmapViewOfFile(void *p) {
        if (p) {
            UnmapViewOfFile(p);
        }
    }

    using HandleGuard = std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(&SafeCloseHandle)>;

    using SecurityDescriptorGuard = std::unique_ptr<std::remove_pointer_t<PSECURITY_DESCRIPTOR>, decltype(&SafeLocalFree)>;

    using MapViewGuard = std::unique_ptr<void, decltype(&SafeUnmapViewOfFile)>;

    static SECURITY_ATTRIBUTES CreateEveryoneAccessSecurity() {
        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES)};
        ConvertStringSecurityDescriptorToSecurityDescriptorA(
                "D:(A;;GA;;;WD)", // allow all users access
                SDDL_REVISION_1,
                &sa.lpSecurityDescriptor,
                nullptr);
        return sa;
    }

    struct HbqjEvents {
        HbqjEvents() :
                update_imguizmo_flag({nullptr, &SafeCloseHandle}) {}

        HandleGuard update_imguizmo_flag;
    };
}