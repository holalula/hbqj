#pragma once

#include <vector>

namespace hbqj {
    class IDecryptionHandler {
    public:
        virtual bool isEncrypted(const std::vector<uint8_t> &data) = 0;

        virtual std::vector<uint8_t> decrypt(const std::vector<uint8_t> &data) = 0;

        virtual ~IDecryptionHandler() = default;
    };
}
