#include <filesystem>
#include <ranges>

#include "utils/string_utils.h"
#include "signature_manager.h"

namespace hbqj {
    /**
     * @brief Get the path to the signature cache file
     * @effects May create the directory if it doesn't exist
     */
    std::filesystem::path SignatureManager::GetCachePath() {
        auto cache_dir = std::filesystem::temp_directory_path() / "hbqj";
        try {
            if (!std::filesystem::exists(cache_dir)) {
                std::filesystem::create_directories(cache_dir);
                log.info("Created cache directory: {}", utf16_to_utf8(cache_dir));
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            log.error("Failed to create cache directory: {}", e.what());
        }
        return cache_dir / "signatures";
    }

    bool SignatureManager::LoadAndVerifyCache(const std::filesystem::path& cache_path, uint64_t current_write_time) {
        log.info("LoadAndVerifyCache, cache path: {}", cache_path.string());
        try {
            if (!std::filesystem::exists(cache_path)) {
                return false;
            }

            std::ifstream file(cache_path);
            auto cache = nlohmann::json::parse(file).get<SignatureCache>();

            if (cache.write_time != current_write_time) {
                return false;
            }

            bool need_update_cache = false;

            signature_db_.clear();

            for (const auto& sig : cache.signatures) {
                // validate cached signature
                std::vector<Byte> memory_data(sig.pattern.size());
                SIZE_T bytes_read;
                if (!ReadProcessMemory(scanner_.process_->target_process_,
                                       reinterpret_cast<LPCVOID>(scanner_.process_->GetBaseAddr() + sig.offset),
                                       memory_data.data(),
                                       sig.pattern.size(),
                                       &bytes_read)) {
                    return false;
                }

                bool is_valid = SignatureScanner::CompareMemory(
                        std::span(memory_data),
                        std::span(sig.pattern),
                        sig.mask);

                if (!is_valid) {
                    // rescan if invalid
                    log.info("Cached signature {} is invalid, rescanning...", GetSigTypeStr(sig.type));
                    const auto& result = scanner_.FindSignature(std::span(sig.pattern), sig.mask);
                    if (!result) {
                        log.error("Failed to find signature {}", GetSigTypeStr(sig.type));
                        continue;
                    }
                    signature_db_[sig.type] = Signature{
                            .type = sig.type,
                            .pattern = std::span(sig.pattern),
                            .mask = sig.mask,
                            .addr = result.value()
                    };
                    need_update_cache = true;
                } else {
                    // use cached result if valid
                    signature_db_[sig.type] = Signature{
                            .type = sig.type,
                            .pattern = std::span(sig.pattern),
                            .mask = sig.mask,
                            .addr = scanner_.process_->GetBaseAddr() + sig.offset
                    };
                }
            }

            if (need_update_cache) {
                log.info("Some signatures were updated, saving new cache...");
                SaveCache();
            } else {
                log.info("All cached signatures are valid");
            }

            return !signature_db_.empty();
        }
        catch (const std::exception& e) {
            log.error("Failed to load signature cache: {}", e.what());
            return false;
        }
    }

    void SignatureManager::SaveCache() {
        try {
            SignatureCache cache;

            HANDLE file_handle = CreateFileW(scanner_.process_->target_module_.path,
                                             GENERIC_READ, FILE_SHARE_READ, nullptr,
                                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (file_handle != INVALID_HANDLE_VALUE) {
                BY_HANDLE_FILE_INFORMATION file_info;
                if (GetFileInformationByHandle(file_handle, &file_info)) {
                    cache.write_time = (static_cast<uint64_t>(file_info.ftLastWriteTime.dwHighDateTime) << 32) |
                                       file_info.ftLastWriteTime.dwLowDateTime;
                }
                CloseHandle(file_handle);
            }

            for (const auto& [type, sig] : signature_db_) {
                cache.signatures.push_back(SignatureCacheItem{
                        .type = sig.type,
                        .pattern = std::vector<Byte>(sig.pattern.begin(), sig.pattern.end()),
                        .mask = std::string(sig.mask),
                        .offset = sig.addr - scanner_.process_->GetBaseAddr()
                });
            }

            std::ofstream file(GetCachePath());
            file << nlohmann::json(cache).dump(2);
            log.info("Successfully saved signature cache in {}", utf16_to_utf8(GetCachePath()));
        }
        catch (const std::exception& e) {
            log.error("Failed to save signature cache: {}", e.what());
        }
    }

	void SignatureManager::Initialize(std::shared_ptr<Process> process) {
		scanner_ = SignatureScanner{};
		scanner_.Initialize(process);

        HANDLE file_handle = CreateFileW(process->target_module_.path,
                                         GENERIC_READ, FILE_SHARE_READ, nullptr,
                                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (file_handle != INVALID_HANDLE_VALUE) {
            log.info("Opened file: {}", utf16_to_utf8(process->target_module_.path));
            BY_HANDLE_FILE_INFORMATION file_info;
            if (GetFileInformationByHandle(file_handle, &file_info)) {
                const auto cache_path = GetCachePath();
                const auto last_write_time = (static_cast<uint64_t>(file_info.ftLastWriteTime.dwHighDateTime) << 32) |
                                             file_info.ftLastWriteTime.dwLowDateTime;

                if (LoadAndVerifyCache(cache_path, last_write_time)) {
                    CloseHandle(file_handle);
                    return;
                }
            }
            CloseHandle(file_handle);
        } else {
            log.error("Failed to open file: {}", utf16_to_utf8(process->target_module_.path));
        }

		for (auto& signature : signatures_) {
			const auto& result = scanner_.FindSignature(signature.pattern, signature.mask);
			if (result) {
				log.info("Found signature {}, address: {:x}", GetSigTypeStr(signature.type), scanner_.process_->GetOffsetAddr(result.value()));
				signature_db_.insert({
					signature.type,
					Signature{.type = signature.type, .pattern = signature.pattern, .mask = signature.mask, .addr = result.value() }
					}
				);
			}
			else {
				log.error("Failed to find signature {}, error code: {}", GetSigTypeStr(signature.type), result.error());
			}
		}

        SaveCache();
	}

	std::expected<const Signature*, Error> SignatureManager::GetSignature(SignatureType type) {
		auto it = signature_db_.find(type);
		if (it == signature_db_.end()) {
			std::string error_message = std::string("Signature not found for type: ").append(GetSigTypeStr(type));
			log.error("{}", error_message);
			return std::unexpected(SignatureNotFoundError{ .message = error_message });
		}
		return &(it->second);
	}
}