#pragma once

#include "struct.h"
#include "log.h"

namespace hbqj {
	class __declspec(dllexport) File {
	public:
        static std::vector<uint8_t> ReadFile(const std::filesystem::path &path) {
            std::ifstream file(path, std::ios::binary);
            if (!file) {
                throw std::runtime_error("Cannot open file: " + path.string());
            }

            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8_t> data(size);
            file.read(reinterpret_cast<char *>(data.data()), size);
            return data;
        }

		Position ReadPosition();
		void SavePosition();
		Logger log = Logger::GetLogger("File");
	};
}