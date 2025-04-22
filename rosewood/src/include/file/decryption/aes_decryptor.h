#pragma once

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <memory>

#include "decryption_handler.h"

namespace hbqj {
    class __declspec(dllexport) AesDecryptor : public IDecryptionHandler {
    public:
        AesDecryptor() : AesDecryptor("wjsycdmm", "neversmile") {};

        AesDecryptor(const std::string &key, const std::string &salt);

        ~AesDecryptor() override;

        bool isEncrypted(const std::vector<uint8_t> &data) override;

        std::vector<uint8_t> decrypt(const std::vector<uint8_t> &data) override;

    private:
        std::vector<unsigned char> key_;
        std::vector<unsigned char> iv_;
    };
}
