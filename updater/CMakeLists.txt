set(PROJECT_NAME updater)

################################################################################
# Source groups
################################################################################
set(ALL_FILES
        "main.cpp"
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

################################################################################
# Include directories
################################################################################
target_include_directories(updater PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/include"
        "${httplib_SOURCE_DIR}"
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
    target_compile_options(${PROJECT_NAME} PRIVATE
            /permissive-
            /W3
    )

    target_link_options(${PROJECT_NAME} PRIVATE
            /DEBUG
            /SUBSYSTEM:CONSOLE
    )
endif ()

################################################################################
# Dependencies
################################################################################
target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)