set(PROJECT_NAME bgpop)

################################################################################
# Source files
################################################################################
set(__ImGui
    "src/ImGui/imgui_impl_dx11.cpp"
    "src/ImGui/imgui_impl_win32.cpp"
)

set(__ImGuizmo
    "src/ImGuizmo/imgui.cpp"
    "src/ImGuizmo/imgui_draw.cpp"
    "src/ImGuizmo/imgui_widgets.cpp"
    "src/ImGuizmo/ImGuizmo.cpp"
)

set(__mhook
    "src/include/mhook/disasm-lib/cpu.c"
    "src/include/mhook/disasm-lib/disasm.c"
    "src/include/mhook/disasm-lib/disasm_x86.c"
    "src/include/mhook/disasm-lib/misc.c"
    "src/include/mhook/mhook-lib/mhook.cpp"
)

set(SOURCE_FILES
    ${__ImGui}
    ${__ImGuizmo}
    ${__mhook}
    "src/dllmain.cpp"
)

################################################################################
# Target
################################################################################
add_library(bgpop SHARED ${SOURCE_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/include/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE bgpop)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
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
            "${CMAKE_CURRENT_SOURCE_DIR}/src/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/include/ImGui;"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/include/ImGuizmo;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../hbqj/include"
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
        "BGPOP_EXPORTS;"
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
   endif()
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(bgpop PUBLIC
    "hbqj"
)

