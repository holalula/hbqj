set(PROJECT_NAME client)

set(__ImGui
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
)

set(SOURCE_FILES
        ${__ImGui}
        ${__ROSEWOOD}
        src/main.cpp
)

if (MSVC)
    add_compile_options(/EHsc)
endif ()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/rosewood/src/include"
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d11
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        rosewood
)
