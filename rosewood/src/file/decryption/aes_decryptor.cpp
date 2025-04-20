#include "file/decryption/aes_decryptor.h"

namespace hbqj {
    std::vector<uint8_t> Base64Decode(const std::vector<uint8_t> &input) {
        BIO *b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO *bmem = BIO_new_mem_buf(input.data(), input.size());
        bmem = BIO_push(b64, bmem);

        std::vector<uint8_t> buffer(input.size());
        int decoded_length = BIO_read(bmem, buffer.data(), input.size());

        BIO_free_all(bmem);

        buffer.resize(decoded_length);
        return buffer;
    }

    std::vector<uint8_t> PadBase64Data(const std::vector<uint8_t> &input) {
        std::vector<uint8_t> padded = input;
        size_t padding_needed = input.size() % 4;
        if (padding_needed > 0) {
            padded.insert(padded.end(), 4 - padding_needed, '=');
        }
        return padded;
    }

    AesDecryptor::AesDecryptor(const std::string &key, const std::string &salt)
            : key_(16, 0), iv_(16, 0) {
        std::copy_n(key.begin(),
                    (std::min)(key.size(), size_t(16)),
                    key_.begin());

        std::copy_n(salt.begin(),
                    (std::min)(salt.size(), size_t(16)),
                    iv_.begin());
    }

    AesDecryptor::~AesDecryptor() = default;

    bool AesDecryptor::isEncrypted(const std::vector<uint8_t> &data) {
        if (data.empty()) return false;

        auto isBase64Char = [](uint8_t c) {
            return (isalnum(c) || c == '+' || c == '/' || c == '=');
        };

        return std::all_of(data.begin(), data.end(), isBase64Char);
    }

    std::vector<uint8_t> AesDecryptor::decrypt(const std::vector<uint8_t> &data) {
        auto padded_data = PadBase64Data(data);
        auto decoded = Base64Decode(padded_data);

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key_.data(), iv_.data());

        // disable default PKCS7 padding
        EVP_CIPHER_CTX_set_padding(ctx, 0);

        std::vector<uint8_t> outbuf(decoded.size() + EVP_MAX_BLOCK_LENGTH);
        int outlen1 = 0;
        EVP_DecryptUpdate(ctx, outbuf.data(), &outlen1,
                          decoded.data(),
                          decoded.size());

        // handle last block
        int outlen2 = 0;
        EVP_DecryptFinal_ex(ctx, outbuf.data() + outlen1, &outlen2);

        EVP_CIPHER_CTX_free(ctx);

        outbuf.resize(outlen1 + outlen2);
        return outbuf;
    }
}