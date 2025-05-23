#pragma once

#include <string>
#include <format>
#include <optional>
#include <windows.h>

#include "shared_memory_struct.h"

namespace hbqj {
    enum EventType {
        UpdateImGuizmoFlag = 0,
        PreviewHousingLayout,
        LoadHousingLayout,
    };

    class EventManager {
    public:
        static std::optional<HandleGuard> CreateHbqjEvent(EventType type) {
            SECURITY_ATTRIBUTES sa = CreateEveryoneAccessSecurity();

            auto handle = ::CreateEventA(
                    &sa,
                    FALSE,
                    FALSE,
                    GetEventName(type).c_str());

            if (handle && handle != INVALID_HANDLE_VALUE) {
                return HandleGuard{handle, &SafeCloseHandle};
            } else {
                return std::nullopt;
            }
        }

        static std::optional<HandleGuard> OpenHbqjEvent(EventType type) {
            auto handle = ::OpenEventA(
                    EVENT_ALL_ACCESS,
                    FALSE,
                    GetEventName(type).c_str());

            if (handle && handle != INVALID_HANDLE_VALUE) {
                return HandleGuard{handle, &SafeCloseHandle};
            } else {
                return std::nullopt;
            }
        }

    private:
        inline static constexpr const char *EVENT_NAME_FORMAT = "Local\\HBQJ_Event_{}";

        static std::string GetEventName(EventType type) {
            switch (type) {
                case UpdateImGuizmoFlag:
                    return std::format(EVENT_NAME_FORMAT, "UpdateImGuizmoFlag");
                case PreviewHousingLayout:
                    return std::format(EVENT_NAME_FORMAT, "PreviewHousingLayout");
                case LoadHousingLayout:
                    return std::format(EVENT_NAME_FORMAT, "LoadHousingLayout");
                default:
                    return "UnknownEvent";
            }
        }
    };
}