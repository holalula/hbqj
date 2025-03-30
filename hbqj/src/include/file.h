#pragma once

#include "struct.h"
#include "log.h"

namespace hbqj {
	class __declspec(dllexport) File {
	public:
		Position ReadPosition();
		void SavePosition();
		Logger log = Logger::GetLogger("File");
	};
}