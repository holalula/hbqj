#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <iostream>
#include <httplib.h>
#include <expected>

#include "version.h"

using namespace std;

static constexpr const char *LATEST_VERSION_HOST = "https://gitee.com";
static constexpr const char *LATEST_VERSION_PATH = "/holalula/hbqj/raw/main/version/latest.json";

std::expected<std::string, httplib::Error> GetRemoteLatestVersionContent() {
    httplib::Client cli(LATEST_VERSION_HOST);

    cli.set_connection_timeout(10);
    cli.set_read_timeout(10);

    auto response = cli.Get(LATEST_VERSION_PATH);

    if (response) {
        std::cout << "Status code: " << response->status << std::endl;
        std::cout << "Response body: " << response->body << std::endl;
        return response->body;
    } else {
        std::cout << "Request failed with error: " << response.error() << std::endl;
        return std::unexpected(response.error());
    }
}

constexpr std::string GetCurrentVersion() {
    return VERSION_STRING;
}


int main() {
    std::cout << "Updater:" << std::endl;

    std::cout << GetCurrentVersion() << std::endl;

    const auto &result = GetRemoteLatestVersionContent();

    if (result) {
        std::cout << result.value() << std::endl;
    } else {
        std::cout << result.error() << std::endl;
    }

    return 0;
}
