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

target_link_libraries(${PROJECT_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/include/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE rosewood)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_KEYWORD "Win32Proj"
)
if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif ()
################################################################################
# Include directories
################################################################################
if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
    )
endif ()

################################################################################
# Compile definitions
################################################################################
if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            "$<$<CONFIG:Debug>:"
            "_DEBUG"
            ">"
            "$<$<CONFIG:Release>:"
            "NDEBUG"
            ">"
            "ROSEWOOD_EXPORTS;"
            "_WINDOWS;"
            "_USRDLL;"
            "UNICODE;"
            "_UNICODE"
    )
endif ()

################################################################################
# Compile and link options
################################################################################
if (MSVC)
    if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:
                /Oi;
                /Gy;
                /Y-;
                /Zi
                >
                /permissive-;
                /sdl;
                /W3;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                ${DEFAULT_CXX_EXCEPTION_HANDLING}
                /FC
        )
    endif ()
    if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
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
    endif ()
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

add_custom_command(TARGET rosewood POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/x64/Release/rosewood.dll"
        "${CMAKE_BINARY_DIR}/rosewood/test/rosewood.dll"
)
