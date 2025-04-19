#pragma once

#include <string>
#include <windows.h>

/// - Functions ending with "W" (like CreateFileW) use UTF-16 (wide characters, wchar_t)
/// - Functions ending with "A" use ANSI (narrow characters, char)
/// - When using the function without W/A (like CreateFile), it maps to W version by default when UNICODE is defined
namespace hbqj {
	inline std::string utf16_to_utf8(const wchar_t* utf16_str) {
		if (!utf16_str) return "";

		// Get required buffer size.
		// UTF-8 is variable-length (1¨C4 bytes per character), 
		// so the buffer size depends on the input string. Pre-calculating ensures safe allocation.
		int size = WideCharToMultiByte(CP_UTF8, 0, utf16_str, -1, nullptr, 0, nullptr, nullptr);
		if (size <= 0) return "";

		// Convert to UTF-8
		std::string utf8_str(size - 1, '\0');  // -1 because size includes null terminator
		WideCharToMultiByte(CP_UTF8, 0, utf16_str, -1, utf8_str.data(), size, nullptr, nullptr);
		return utf8_str;
	}

	// Overload for std::wstring
	inline std::string utf16_to_utf8(const std::wstring& utf16_str) {
		return utf16_to_utf8(utf16_str.c_str());
	}
}