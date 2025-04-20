#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>

#include "file/decryption/aes_decryptor.h"

namespace hbqj {
    std::vector<uint8_t> read_file(const std::filesystem::path &path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + path.string());
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(size);
        file.read(reinterpret_cast<char *>(data.data()), size);
        return data;
    }

    TEST(DecryptionTest, Test) {
        AesDecryptor decryptor("wjsycdmm", "neversmile");

        const auto &current_dir = std::filesystem::current_path();
        const auto &test_file = current_dir / "test.hbqj";

        auto encrypted_data = read_file(test_file);

        if (decryptor.isEncrypted(encrypted_data)) {
            auto decrypted = decryptor.decrypt(encrypted_data);

            std::ofstream out_file("decrypted.txt", std::ios::binary);
            out_file.write(reinterpret_cast<const char *>(decrypted.data()), decrypted.size());
        }
    }
}