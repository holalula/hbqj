// clear redundant functions
#pragma once
#include <vector>
#include "../ff/FFLog.h"
namespace Memory {
	//ReadWriteMemory
	template<typename ReadType>
	ReadType ReadGameMemory(HANDLE hProcess, SIZE_T addr) {
		ReadType buf;
		SIZE_T readSz;
		if (hProcess == NULL) {
			Log(FFLog::LogType::error) << "[ERROR] ReadGameMemory failed to open process" << std::endl;
			std::cout << "[ERROR] ReadGameMemory failed to open process" << std::endl;
			return NULL;
		}
		ReadProcessMemory(hProcess, (LPCVOID)addr, &buf, sizeof(ReadType), &readSz);
		if (readSz != sizeof(ReadType)) {
			Log(FFLog::LogType::warn) << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
			std::cout << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
		}
		return buf;
	}
	template<typename ReadType>
	ReadType ReadGameMemory(HANDLE hProcess, SIZE_T addr, std::vector<SIZE_T>offsets) {
		int offset_size = offsets.size();
		ReadType buf;
		SIZE_T dwSize;
		SIZE_T value1;
		SIZE_T value2;
		value1 = addr;
		for (int i = 0; i < offset_size - 1; i++) {
			SIZE_T offset = offsets[i];
			ReadProcessMemory(hProcess, (LPVOID)(value1 + offset), &value2, sizeof(SIZE_T), &dwSize);
			value1 = value2;
		}
		SIZE_T final_add = value1 + offsets.back();
		ReadProcessMemory(hProcess, (LPVOID)final_add, &buf, sizeof(buf), &dwSize);
		if (dwSize != sizeof(ReadType)) {
			Log(FFLog::LogType::warn) << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
			std::cout << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
		}
		return buf;
	}

	template<typename WriteType>
	WriteType WriteGameMemory(HANDLE hProcess, SIZE_T addr, WriteType writeData, std::vector<SIZE_T>offsets, SIZE_T final_addr_offset) {
		int offset_size = offsets.size();
		SIZE_T dwSize;
		SIZE_T value1;
		SIZE_T value2;
		value1 = addr;
		for (int i = 0; i < offset_size - 1; i++) {
			SIZE_T offset = offsets[i];
			ReadProcessMemory(hProcess, (LPVOID)(value1 + offset), &value2, sizeof(SIZE_T), &dwSize);
			value1 = value2;
		}
		SIZE_T final_add = value1 + offsets.back();
		WriteProcessMemory(hProcess, (LPVOID)(final_add + final_addr_offset), &writeData, sizeof(writeData), &dwSize);
		if (dwSize != sizeof(WriteType)) {
			Log(FFLog::LogType::warn) << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
			std::cout << "[WARN] WriteGameMemory dwsize is different from WriteType size" << std::endl;
		}
		return writeData;
	}
	template<typename WriteType>
	WriteType WriteGameMemory(HANDLE hProcess, SIZE_T addr, WriteType writeData, std::vector<SIZE_T>offsets) {
		int offset_size = offsets.size();
		SIZE_T dwSize;
		SIZE_T value1;
		SIZE_T value2;
		//SIZE_T readSz;
		//ReadProcessMemory(hProcess,(LPVOID)addr, &value1, sizeof(SIZE_T), &dwSize);
		value1 = addr;
		for (int i = 0; i < offset_size - 1; i++) {
			SIZE_T offset = offsets[i];
			ReadProcessMemory(hProcess, (LPVOID)(value1 + offset), &value2, sizeof(SIZE_T), &dwSize);
			value1 = value2;
		}
		SIZE_T final_add = value1 + offsets.back();
		WriteProcessMemory(hProcess, (LPVOID)final_add, &writeData, sizeof(writeData), &dwSize);
		if (dwSize != sizeof(WriteType)) {
			Log(FFLog::LogType::warn) << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
			std::cout << "[WARN] WriteGameMemory dwsize is different from WriteType size" << std::endl;
		}
		return writeData;
	}
	template<typename ReadType>
	std::vector<ReadType> ReadGameMemory(HANDLE hProcess, SIZE_T addr, std::vector<SIZE_T>offsets, std::vector<SIZE_T>final_addr_offsets) {
		int read_count = final_addr_offsets.size();
		std::vector<ReadType> return_v;

		int offset_size = offsets.size();
		ReadType buf;
		SIZE_T dwSize;
		SIZE_T value1;
		SIZE_T value2;
		value1 = addr;
		for (int i = 0; i < offset_size - 1; i++) {
			SIZE_T offset = offsets[i];
			ReadProcessMemory(hProcess, (LPVOID)(value1 + offset), &value2, sizeof(SIZE_T), &dwSize);
			value1 = value2;
		}
		SIZE_T final_add = value1 + offsets.back();

		for (int i = 0; i < read_count; i++) {
			ReadProcessMemory(hProcess, (LPVOID)(final_add + final_addr_offsets[i]), &buf, sizeof(buf), &dwSize);
			return_v.push_back(buf);
		}

		if (dwSize != sizeof(ReadType)) {
			Log(FFLog::LogType::warn) << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
			std::cout << "[WARN] ReadGameMemory readsize is different from ReadType size" << std::endl;
		}
		return return_v;
	}
}