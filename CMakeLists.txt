cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)
project(hbqj LANGUAGES C CXX)

set(CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_CXX_STANDARD 23)

################################################################################
# Project Version
################################################################################
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

configure_file(
        ${CMAKE_SOURCE_DIR}/include/version.h.in
        ${CMAKE_BINARY_DIR}/include/version.h
)

################################################################################
# Third-party Dependencies
################################################################################
include(FetchContent)

FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

FetchContent_declare(
        mhook
        GIT_REPOSITORY https://github.com/holalula/mhook.git
        GIT_TAG 92d195dbdfd23c5ff6faf7412b0a64afe2966624
)
set(MHOOK_TESTS OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(mhook)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG a26fcf58cc2d0cfeee8b98ad8517af998f75c27e
)
FetchContent_Populate(imgui)

FetchContent_Declare(
        imguizmo
        GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
        GIT_TAG 2310acda820d7383d4c4884b7945ada92cd16a47
)
FetchContent_Populate(imguizmo)

set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_MSVC_STATIC_RT TRUE)
find_package(OpenSSL REQUIRED)
if (NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL.")
endif ()

FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG 3af7f2c16147f3fbc6e4d717032daf505dc1652c
)
FetchContent_Populate(httplib)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(namazu)
add_subdirectory(rosewood)
add_subdirectory(updater)
add_subdirectory(client)
