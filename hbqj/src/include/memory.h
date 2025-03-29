#pragma once

#include <expected>
#include <optional>

#include "error.h"
#include "signature_manager.h"
#include "process.h"
#include "macro.h"

namespace hbqj {
#pragma pack(push, 1)
	struct Position {
		float x;
		float y;
		float z;
	};

	struct Quaternion {
		float x;
		float y;
		float z;
		float w;
	};
#pragma pack(pop)

	class __declspec(dllexport) Memory {
	public:
		std::expected<void, Error> Initialize(std::shared_ptr<Process> process) {
			process_ = process;
			signature_manager_.Initialize(process_);
			return {};
		}

		std::expected<void, Error> PlaceAnywhere(bool enable);

		std::shared_ptr<Process> process_;

		std::expected<Address, Error> GetActiveHousingItem();

		std::expected<Position, Error> GetActivePosition();

		std::expected<Quaternion, Error> GetActiveRotation();

		std::expected<Position, Error> SetActivePosition(std::optional<float> x, std::optional<float> y, std::optional<float> z);
	private:
		SignatureManager signature_manager_;
		Logger log = Logger::GetLogger("Memory");
	};
}