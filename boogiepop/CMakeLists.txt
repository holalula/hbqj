set(PROJECT_NAME boogiepop)

################################################################################
# Source groups
################################################################################
set(__ImGui
    "include/ImGui/imgui_impl_dx11.h"
    "include/ImGui/imgui_impl_win32.h"
)

set(__ImGuizmo
    "include/ImGuizmo/imgui.h"
    "include/ImGuizmo/imgui_internal.h"
    "include/ImGuizmo/ImGuizmo.h"
    "include/ImGuizmo/imstb_rectpack.h"
    "include/ImGuizmo/imstb_textedit.h"
    "include/ImGuizmo/imstb_truetype.h"
    "include/ImGuizmo/stb_image.h"
)

set(__mhook
    "include/mhook/disasm-lib/cpu.h"
    "include/mhook/disasm-lib/disasm.h"
    "include/mhook/disasm-lib/disasm_x86.h"
    "include/mhook/disasm-lib/disasm_x86_tables.h"
    "include/mhook/disasm-lib/misc.h"
    "include/mhook/mhook-lib/mhook.h"
)

set(
    "dllmain.cpp"
)

set(__ImGui
    "ImGui/imgui_impl_dx11.cpp"
    "ImGui/imgui_impl_win32.cpp"
)

set(__ImGuizmo
    "ImGuizmo/imgui.cpp"
    "ImGuizmo/imgui_draw.cpp"
    "ImGuizmo/imgui_widgets.cpp"
    "ImGuizmo/ImGuizmo.cpp"
)

set(__mhook
    "include/mhook/disasm-lib/cpu.c"
    "include/mhook/disasm-lib/disasm.c"
    "include/mhook/disasm-lib/disasm_x86.c"
    "include/mhook/disasm-lib/misc.c"
    "include/mhook/mhook-lib/mhook.cpp"
)

set(ALL_FILES
    ${__ImGui}
    ${__ImGuizmo}
    ${__mhook}
    ${}
    ${__ImGui}
    ${__ImGuizmo}
    ${__mhook}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE boogiepop)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/ImGui;"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/ImGuizmo;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../HaiBinQiangJia/include"
        ">"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "BOOGIEPOP_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "BOOGIEPOP_EXPORTS;"
        "_WINDOWS;"
        "_USRDLL;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    HaiBinQiangJia
)

