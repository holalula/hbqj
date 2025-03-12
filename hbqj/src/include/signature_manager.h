#pragma once

#include <array>
#include <expected>
#include <string>
#include <unordered_map>

#include "error.h"

namespace hbqj {
	typedef unsigned __int64 Address;

	enum class SignatureType {
		PA1,
		PA2,
		PA3,
		Blue,
		OperateItem,
		LoadHousem,
		ViewMatrix,
		BaseHouse,
		SavePreview,
		SelectFunction,
		LayoutWorld,
		ActorTable,
		MousingModule,
		DX_Present,
		SIGNATURE_TYPE_COUNT,
	};

	struct Signature {
		SignatureType type;
		const char* pattern;
		const char* mask;
		Address addr;
	};

	class SignatureManager {
	public:
		void initialize();
		std::expected<void, Error> scan_signatures();
		std::expected<const Signature&, Error> get_signature(SignatureType type);

	private:
		inline static const std::array<Signature, static_cast<size_t>(SignatureType::SIGNATURE_TYPE_COUNT)> signatures_ = {
			{
				SignatureType::PA1,
				"",
				"",
			}
		};

		std::unordered_map<SignatureType, Signature> signature_db_;
	};
}