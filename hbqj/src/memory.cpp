#include "memory.h"

namespace hbqj {
	std::expected<void, Error> Memory::PlaceAnywhere(bool enable) {
		TRY(pa1,
			signature_manager_.GetSignature(SignatureType::PA1));
		TRY(pa2,
			signature_manager_.GetSignature(SignatureType::PA2));
		TRY(pa3,
			signature_manager_.GetSignature(SignatureType::PA3));

		Byte value = enable ? 1 : 0;

		TRY(_1, process_->WriteMemory<Byte>(pa1->addr + 6, value));
		TRY(_2, process_->WriteMemory<Byte>(pa2->addr + 11, value));
		TRY(_3, process_->WriteMemory<Byte>(pa3->addr + 6, value));
	}

	std::expected<Address, Error> Memory::GetActiveHousingItem() {
		TRY(housing_base_signature,
			signature_manager_.GetSignature(SignatureType::BaseHouse));
		TRY(housing_base_offset,
			process_->CalculateTargetOffsetMov(housing_base_signature->addr - process_->GetBaseAddr()));
		TRY(housing_addr,
			process_->ReadMemory<Address>(process_->GetBaseAddr() + housing_base_offset));
		TRY(housing_addr1,
			process_->ReadMemory<Address>(housing_addr + 0x40));
		TRY(active_housing_item_addr,
			process_->ReadMemory<Address>(housing_addr1 + 0x18));

		if (!active_housing_item_addr) {
			return std::unexpected(NullPointerError{ .message = "active_housing_item" });
		}

		return active_housing_item_addr;
	}

	std::expected<float, Error> Memory::GetActivePositionX() {
		TRY(active_housing_item,
			GetActiveHousingItem());

		TRY(v,
			process_->ReadMemory<float>(active_housing_item + 0x50));

		return v;
	}

	std::expected<float, Error> Memory::GetActivePositionY() {
		TRY(active_housing_item,
			GetActiveHousingItem());

		TRY(v,
			process_->ReadMemory<float>(active_housing_item + 0x50 + 0x4));

		return v;
	}

	std::expected<float, Error> Memory::GetActivePositionZ() {
		TRY(active_housing_item,
			GetActiveHousingItem());

		TRY(v,
			process_->ReadMemory<float>(active_housing_item + 0x50 + 0x4 + 0x4));

		return v;
	}

	std::expected<Quaternion, Error> Memory::GetActiveRotation() {
		TRY(active_housing_item,
			GetActiveHousingItem());

		TRY(v,
			process_->ReadMemory<Quaternion>(active_housing_item + 0x60));

		return v;
	}

	std::expected<Position, Error> Memory::GetActivePosition() {
		TRY(x,
			GetActivePositionX());
		TRY(y,
			GetActivePositionY());
		TRY(z,
			GetActivePositionZ());

		return Position{ .x = x, .y = y, .z = z };
	}
}