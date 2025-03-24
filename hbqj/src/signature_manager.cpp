#include <ranges>

#include "signature_manager.h"

namespace hbqj {
	void SignatureManager::Initialize(std::shared_ptr<Process> process) {
		scanner_ = SignatureScanner{};

		scanner_.Initialize(process);
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