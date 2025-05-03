set(PROJECT_NAME rosewood)

################################################################################
# Source files
################################################################################
set(SOURCE_FILES
        "src/dllmain.cpp"
        "src/signature_scanner.cpp"
        "src/signature_manager.cpp"
        "src/process.cpp"
        "src/memory_operation.cpp"
        "src/file.cpp"
        "src/hook.cpp"
        "src/file/decryption/aes_decryptor.cpp"
)

################################################################################
# Target
################################################################################
add_library(rosewood SHARED ${SOURCE_FILES})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(rosewood PRIVATE
        _WINDOWS
        _USRDLL
        UNICODE
        _UNICODE
)

################################################################################
# Compile and link options
################################################################################
if (MSVC)
    target_compile_options(rosewood PRIVATE
            /permissive-
            /W3
    )

    target_link_options(rosewood PRIVATE
            /DEBUG
            /SUBSYSTEM:WINDOWS
    )
endif ()

################################################################################
# Dependencies
################################################################################
target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

add_custom_command(TARGET rosewood POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/rosewood/rosewood.dll"
        "${CMAKE_BINARY_DIR}/rosewood/test/rosewood.dll"
)
