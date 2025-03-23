#pragma once

#include <array>
#include <expected>
#include <string>
#include <string_view>
#include <unordered_map>

#include "log.h"
#include "signature_scanner.h"
#include "error.h"

namespace hbqj {
	enum class SignatureType {
		PA1,
		PA2,
		PA3,
		Blue,
		OperateItem,
		LoadHouse,
		ViewMatrix,
		BaseHouse,
		SavePreview,
		SelectFunction,
		LayoutWorld,
		ActorTable,
		HousingModule,
		DX_Present,
		COUNT,
	};

	struct Signature {
		SignatureType type;
		std::span<const Byte> pattern;
		std::string_view mask;
		Address addr;
	};

	class __declspec(dllexport) SignatureManager {
	public:
		void Initialize();
		std::expected<const Signature*, Error> GetSignature(SignatureType type);

		inline std::string_view GetSigTypeStr(SignatureType type) {
			switch (type) {
			case SignatureType::PA1: return "PA1";
			case SignatureType::PA2: return "PA2";
			case SignatureType::PA3: return "PA3";
			case SignatureType::Blue: return "Blue";
			case SignatureType::OperateItem: return "OperateItem";
			case SignatureType::LoadHouse: return "LoadHouse";
			case SignatureType::ViewMatrix: return "ViewMatrix";
			case SignatureType::BaseHouse: return "BaseHouse";
			case SignatureType::SavePreview: return "SavePreview";
			case SignatureType::SelectFunction: return "SelectFunction";
			case SignatureType::LayoutWorld: return "LayoutWorld";
			case SignatureType::ActorTable: return "ActorTable";
			case SignatureType::HousingModule: return "HousingModule";
			case SignatureType::DX_Present: return "DX_Present";
			default: return "Unknown";
			}
		}

	private:
		inline static const std::array<Signature, static_cast<size_t>(SignatureType::COUNT)> signatures_ = { {
			{
				SignatureType::PA1,
				SignatureScanner::MakePattern("\xC6\x00\x00\x00\x00\x00\x00\x8B\xFE\x48\x89"),
				"x???xxxxxxx"
			},
			{
				SignatureType::PA2,
				SignatureScanner::MakePattern("\x48\x85\xC0\x74\x00\xC6\x87\x00\x00\x00\x00\x00"),
				"xxxx?xx??xxx"
			},
			{
				SignatureType::PA3,
				SignatureScanner::MakePattern("\xC6\x87\x83\x01\x00\x00\x00\x48\x83\xC4\x00"),
				"xxxxxxxxxx?"
			},
			{
				SignatureType::Blue,
				SignatureScanner::MakePattern("\x48\x85\x00\x0F\x84\x00\x00\x00\x00\x48\x89\x00\x00\x00\x57\x48\x83\xEC\x00\x48\x8B\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x33\xD2"),
				"xx?xx????xx???xxxx?xx?xx?x????xx"
			},
			{
				SignatureType::OperateItem,
				SignatureScanner::MakePattern("\x48\x85\xD2\x0F\x84\x49\x09\x00\x00\x53\x41\x56\x48\x83\xEC\x48\x48\x89\x6C\x24\x60\x48\x8B\xDA\x48\x89\x74\x24\x70\x4C\x8B\xF1"),
				"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
			},
			{
				SignatureType::LoadHouse,
				SignatureScanner::MakePattern("\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x48\x89\x7C\x24\x00\x41\x56\x48\x83\xEC\x20\x48\x8B\x71\x08\x48\x8B\xFA"),
				"xxxx?xxxx?xxxx?xxxx?xxxxxxxxxxxxx"
			},
			{
				SignatureType::ViewMatrix,
				SignatureScanner::MakePattern("\xE8\x00\x00\x00\x00\x48\x8D\x4C\x24\x00\x48\x89\x4C\x24\x00\x4C\x8D\x4D\x00\x4C\x8D\x44\x24\x00"),
				"x????xxxx?xxxx?xxx?xxxx?"
			},
			{
				SignatureType::BaseHouse,
				SignatureScanner::MakePattern("\x48\x8B\x00\x00\x00\x00\x00\x48\x85\x00\x74\x00\x48\x8B\x00\x00\xE9\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC"),
				"xx?????xx?x?xx??x????xxxxxxxxxxxx"
			},
			{
				SignatureType::SavePreview,
				SignatureScanner::MakePattern("\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x55\x48\x8D\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33\x00\x48\x89\x00\x00\x0F\x10"),
				"xx???xx???xxx???xxx????xx?????xx?xx??xx"
			},
			{
				SignatureType::SelectFunction,
				SignatureScanner::MakePattern("\xE8\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x48\x8B\x00\xE8"),
				"x????xx?x????xx???xx?x"
			},
			{
				SignatureType::LayoutWorld,
				SignatureScanner::MakePattern("\x48\x8B\x00\x00\x00\x00\x00\x48\x85\x00\x74\x00\x48\x8B\x00\x00\xE9\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC"),
				"xx?????xx?x?xx??x????xxxxxxxxxxxx"
			},
			{
				SignatureType::ActorTable,
				SignatureScanner::MakePattern("\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x0F\xB6\x83\x00\x00\x00\x00\xC6\x83"),
				"xxx????x????xxxx????xx"
			},
			{
				SignatureType::HousingModule,
				SignatureScanner::MakePattern("\x48\x8B\x05\x00\x00\x00\x00\x8B\x52"),
				"xxx????xx"
			},
			{
				// TODO: hook dxgi present by vtable
				SignatureType::DX_Present,
				SignatureScanner::MakePattern("\x41\x8B\xF0\x8B\xFA\x89\x54\x24\x00\x48\x8B\xD9\x48\x89\x4D\x00\xC6\x44\x24\x00\x00"),
				"xxxxxxxx?xxxxxx?xxx?x"
			}
		} };

		std::unordered_map<SignatureType, Signature> signature_db_;
		SignatureScanner scanner_;
		Logger log = Logger::GetLogger("SignatureManager");
	};
}