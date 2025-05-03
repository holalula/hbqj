set(PROJECT_NAME namazu)

################################################################################
# Source files
################################################################################
set(__IMGUI
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
)

set(__IMGUIZMO
    "${imguizmo_SOURCE_DIR}/ImGuizmo.cpp"
)

set(__ROSEWOOD
    "${CMAKE_SOURCE_DIR}/rosewood/src/process.cpp"
    "${CMAKE_SOURCE_DIR}/rosewood/src/signature_manager.cpp"
    "${CMAKE_SOURCE_DIR}/rosewood/src/signature_scanner.cpp"
    "${CMAKE_SOURCE_DIR}/rosewood/src/memory_operation.cpp"
)

set(SOURCE_FILES
    ${__IMGUI}
    ${__IMGUIZMO}
    ${__ROSEWOOD}
    "src/dllmain.cpp"
)

################################################################################
# Target
################################################################################
add_library(namazu SHARED ${SOURCE_FILES})

################################################################################
# Include directories
################################################################################
target_include_directories(namazu PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
        "${CMAKE_SOURCE_DIR}/rosewood/src/include"
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
        "${mhook_SOURCE_DIR}"
        "${imguizmo_SOURCE_DIR}"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(namazu PRIVATE
        _WINDOWS
        _USRDLL
        UNICODE
        _UNICODE
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(namazu PRIVATE
            /permissive-
            /W3
    )

    target_link_options(namazu PRIVATE
            /DEBUG
            /SUBSYSTEM:WINDOWS
    )
endif()

################################################################################
# Dependencies
################################################################################
target_link_libraries(namazu PUBLIC
    nlohmann_json::nlohmann_json
    mhook
    d3d11.lib
)
