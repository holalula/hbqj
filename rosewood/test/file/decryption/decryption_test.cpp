#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>

#include "file.h"
#include "file/decryption/aes_decryptor.h"

namespace hbqj {
    TEST(DecryptionTest, ReadString) {
        const auto& data = File::ReadFile(std::filesystem::current_path() / "hbqj.json");

        // utf-8 by default
        // TODO: make sure to always use utf-8 when reading and writing files
        const auto& str = std::string(data.begin(), data.end());

        std::cout << str << std::endl;
    }

    TEST(DecryptionTest, Test) {
        AesDecryptor decryptor("wjsycdmm", "neversmile");

        const auto &current_dir = std::filesystem::current_path();
        const auto &test_file = current_dir / "test.hbqj";

        auto encrypted_data = File::ReadFile(test_file);

        if (decryptor.isEncrypted(encrypted_data)) {
            auto decrypted = decryptor.decrypt(encrypted_data);

            std::ofstream out_file("decrypted.txt", std::ios::binary);
            out_file.write(reinterpret_cast<const char *>(decrypted.data()), decrypted.size());
        }
    }
}