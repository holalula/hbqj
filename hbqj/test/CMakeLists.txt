enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

 add_executable(hbqj_tests
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
 )

# add_dependencies(hbqj_tests namazu)

target_link_libraries(hbqj_tests
    PRIVATE
    gtest_main
    hbqj
)

target_include_directories(hbqj_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)

include(GoogleTest)
gtest_discover_tests(hbqj_tests)
