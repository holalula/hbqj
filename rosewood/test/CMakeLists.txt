enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

if (MSVC)
    add_compile_options(/EHsc)
endif ()

add_executable(rosewood_tests
        main_test.cpp
        signature_manager_test.cpp
        utils/string_utils_test.cpp
        signature_scanner_test.cpp
        signature_manager_test.cpp
        log_test.cpp
        memory_test.cpp
        macro_test.cpp
        file_test.cpp
        hook_test.cpp
        file/decryption/decryption_test.cpp
        file/deserialization/deserialization_test.cpp
        file/file_reader_test.cpp
        layout_loader_test.cpp
        ipc_test.cpp
)

# add_dependencies(rosewood_tests namazu)

target_link_libraries(rosewood_tests
        PRIVATE
        gtest_main
        rosewood
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_include_directories(rosewood_tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

include(GoogleTest)
gtest_discover_tests(rosewood_tests)
