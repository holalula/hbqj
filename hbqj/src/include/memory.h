#pragma once

#include <expected>
#include <format>
#include <optional>
#include <vector>

#include "error.h"
#include "signature_manager.h"
#include "process.h"
#include "macro.h"

namespace hbqj {
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

	struct HousingItem {
		HousingItem(uint32_t t = 0, Position p = Position{ 0, 0, 0 }, float r = 0.0f, Byte c = 0)
			: type(t), position(p), rotation(r), color(c) {
		}

		uint32_t type;
		Position position;
		float rotation;
		Byte color;
	};

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

		std::expected<std::vector<HousingItem>, Error> GetFurnitureList();
	private:
		SignatureManager signature_manager_;
		Logger log = Logger::GetLogger("Memory");
	};
}

template<>
struct std::formatter<hbqj::Position> : std::formatter<std::string> {
	auto format(const hbqj::Position& pos, format_context& ctx) const {
		return format_to(ctx.out(), "Position(x={:.2f}, y={:.2f}, z={:.2f})", pos.x, pos.y, pos.z);
	}
};

template<>
struct std::formatter<hbqj::HousingItem> : std::formatter<std::string> {
	auto format(const hbqj::HousingItem& item, format_context& ctx) const {
		return format_to(ctx.out(), "HousingItem(type={}, position={}, rotation={:.2f}, color={})",
			item.type, item.position, item.rotation, static_cast<int>(item.color));
	}
};