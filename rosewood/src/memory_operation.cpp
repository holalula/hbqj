#include "memory_operation.h"

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
            return std::unexpected(NullPointerError{.message = "active_housing_item"});
        }

        return active_housing_item_addr;
    }

    std::expected<int32_t, Error> Memory::GetLayoutMode() {
        TRY(housing_base_signature,
            signature_manager_.GetSignature(SignatureType::BaseHouse));
        TRY(housing_base_offset,
            process_->CalculateTargetOffsetMov(housing_base_signature->addr - process_->GetBaseAddr()));
        TRY(housing_addr,
            process_->ReadMemory<Address>(process_->GetBaseAddr() + housing_base_offset));
        TRY(housing_addr1,
            process_->ReadMemory<Address>(housing_addr + 0x40));

        TRY(mode,
            process_->ReadMemory<int32_t>(housing_addr1));

        return mode;
    }

    std::expected<int64_t, Error> Memory::GetHousingStructureAddr() {
        TRY(housing_base_signature,
            signature_manager_.GetSignature(SignatureType::BaseHouse));
        TRY(housing_base_offset,
            process_->CalculateTargetOffsetMov(housing_base_signature->addr - process_->GetBaseAddr()));
        TRY(layout_world_addr,
            process_->ReadMemory<Address>(process_->GetBaseAddr() + housing_base_offset));

        TRY(housing_structure_addr,
            process_->ReadMemory<Address>(layout_world_addr + 0x40));

        return housing_structure_addr;
    }

    std::expected<Quaternion, Error> Memory::GetActiveRotation() {
        TRY(active_housing_item,
            GetActiveHousingItem());

        TRY(v,
            process_->ReadMemory<Quaternion>(active_housing_item + 0x60));

        return v;
    }

    std::expected<Position, Error> Memory::GetActivePosition() {
        TRY(active_housing_item,
            GetActiveHousingItem());

        TRY(position,
            process_->ReadMemory<Position>(active_housing_item + 0x50));

        return position;
    }

    std::expected<Position, Error>
    Memory::SetActivePosition(std::optional<float> x, std::optional<float> y, std::optional<float> z) {
        TRY(position,
            GetActivePosition());

        if (x) {
            position.x = x.value();
        }

        if (y) {
            position.y = y.value();
        }

        if (z) {
            position.z = z.value();
        }

        TRY(active_housing_item,
            GetActiveHousingItem());

        TRY(_, process_->WriteMemory<Position>(active_housing_item + 0x50, position));

        return position;
    }

    std::expected<std::vector<HousingItem>, Error> Memory::GetFurnitureList() {
        TRY(housing_module_signature,
            signature_manager_.GetSignature(SignatureType::HousingModule));

        TRY(housing_module_offset,
            process_->CalculateTargetOffsetMov(housing_module_signature->addr - process_->GetBaseAddr()));

        TRY(housing_module_addr,
            process_->ReadMemory<Address>(housing_module_offset + process_->GetBaseAddr()));

        TRY(indoor_housing_module,
            process_->ReadMemory<Address>(housing_module_addr + 0x10));

        if (!indoor_housing_module) {
            return std::unexpected(NullPointerError{.message = "indoor_housing_module"});
        }

        const auto item_start_addr = indoor_housing_module + 0x8980;

        std::vector<HousingItem> items;
        for (int i = 0; i < 400; i++) {
            Address item_ptr_addr = item_start_addr + i * sizeof(Address);
            if (!item_ptr_addr) continue;
            const auto &item_addr = process_->ReadMemory<Address>(item_ptr_addr);
            if (!item_addr) continue;

            const auto &type_result = process_->ReadMemory<uint32_t>(item_addr.value() + 0x80);
            if (!type_result) continue;
            const auto &position_result = process_->ReadMemory<Position>(item_addr.value() + 0xB0);
            if (!position_result) continue;
            const auto &rotation_result = process_->ReadMemory<float>(item_addr.value() + 0xC0);
            if (!rotation_result) continue;
            const auto &color_result = process_->ReadMemory<Byte>(item_addr.value() + 0x1B0);
            if (!color_result) continue;
            const auto &item_addr_result = process_->ReadMemory<Address>(item_addr.value() + 0x108);
            if (!item_addr_result) continue;

            items.emplace_back(
                    type_result.value(),
                    position_result.value(),
                    rotation_result.value(),
                    color_result.value(),
                    item_addr_result.value()
            );
        }

        return items;
    }
}