#pragma once

#include <expected>
#include <format>
#include <optional>
#include <vector>

#include "error.h"
#include "signature_manager.h"
#include "struct.h"
#include "process.h"
#include "macro.h"


namespace hbqj {
    enum HousingLayoutMode {
        None = 0,
        Move = 1,
        Rotate = 2,
    };

	class __declspec(dllexport) Memory {
	public:
		std::expected<void, Error> Initialize(std::shared_ptr<Process> process) {
			process_ = process;
			signature_manager_.Initialize(process_);
            initialized = true;
			return {};
		}

		std::expected<void, Error> PlaceAnywhere(bool enable);

		std::shared_ptr<Process> process_;

		std::expected<Address, Error> GetActiveHousingItem();

        std::expected<int32_t, Error> GetLayoutMode();

        std::expected<int64_t, Error> GetHousingStructureAddr();

		std::expected<Position, Error> GetActivePosition();

		std::expected<Quaternion, Error> GetActiveRotation();

		std::expected<Position, Error> SetActivePosition(std::optional<float> x, std::optional<float> y, std::optional<float> z);

		std::expected<std::vector<HousingItem>, Error> GetFurnitureList();

        bool initialized = false;
	private:
		SignatureManager signature_manager_;
		Logger log = Logger::GetLogger("Memory");
	};
}
