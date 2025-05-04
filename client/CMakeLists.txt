set(PROJECT_NAME client)

################################################################################
# Source groups
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

set(__ROSEWOOD
        "${CMAKE_SOURCE_DIR}/rosewood/src/process.cpp"
        "${CMAKE_SOURCE_DIR}/rosewood/src/file/decryption/aes_decryptor.cpp"
        "${CMAKE_SOURCE_DIR}/rosewood/src/hook.cpp"
        "${CMAKE_SOURCE_DIR}/rosewood/src/signature_manager.cpp"
        "${CMAKE_SOURCE_DIR}/rosewood/src/signature_scanner.cpp"
        "${CMAKE_SOURCE_DIR}/rosewood/src/memory_operation.cpp"
)

set(SOURCE_FILES
        ${__IMGUI}
        ${__ROSEWOOD}
        src/main.cpp
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/rosewood/src/include"
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
        UNICODE
        _UNICODE
)

################################################################################
# Compile and link options
################################################################################
if (MSVC)
    # https://learn.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model
    add_compile_options(/EHsc)
endif ()

################################################################################
# Dependencies
################################################################################
target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d11
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)

add_dependencies(client namazu)

add_custom_command(TARGET client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/namazu/namazu.dll"
        "${CMAKE_BINARY_DIR}/client/namazu.dll"
)
