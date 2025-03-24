#include "memory.h"

namespace hbqj {
	std::expected<void, Error> Memory::PlaceAnywhere(bool enable) {
		TRY(pa1, signature_manager_.GetSignature(SignatureType::PA1));
		TRY(pa2, signature_manager_.GetSignature(SignatureType::PA2));
		TRY(pa3, signature_manager_.GetSignature(SignatureType::PA3));

		Byte value = enable ? 1 : 0;

		// TODO: handle error
		process_->WriteMemory<Byte>(pa1.addr + 6, value);
		process_->WriteMemory<Byte>(pa2.addr + 11, value);
		process_->WriteMemory<Byte>(pa3.addr + 6, value);
	}
}