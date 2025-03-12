#include <gtest/gtest.h>
#include <iostream>

#include "utils/string_utils.h"

using hbqj::utf16_to_utf8;

TEST(Utf16ToUtf8Test, NullPointerReturnsEmpty) {
	const wchar_t* input = nullptr;
	std::string result = utf16_to_utf8(input);
	EXPECT_TRUE(result.empty());
}

TEST(Utf16ToUtf8Test, EmptyStringConvertsToEmpty) {
	std::string result = utf16_to_utf8(L"");
	EXPECT_EQ(result, "");
}

TEST(Utf16ToUtf8Test, BasicAsciiConversion) {
	std::string result = utf16_to_utf8(L"Hello World!");
	EXPECT_EQ(result, "Hello World!");
}

TEST(Utf16ToUtf8Test, UnicodeCharacters) {
	// Test Latin-1 Supplement: 'иж' (U+00E9)
	std::string result = utf16_to_utf8(L"\u00E9");
	EXPECT_EQ(result, "\xC3\xA9");

	// Test Currency Symbols: Euro sign (U+20AC)
	result = utf16_to_utf8(L"\u20AC");
	EXPECT_EQ(result, "\xE2\x82\xAC");

	// Test Emoji: Grinning face (U+1F600)
	result = utf16_to_utf8(L"\U0001F600");
	EXPECT_EQ(result, "\xF0\x9F\x98\x80");
}

TEST(Utf16ToUtf8Test, StdWstringOverloadEquivalence) {
	// Test equivalence with const wchar_t* version
	std::wstring ws = L"Test \u00E9\U0001F600";
	std::string result1 = utf16_to_utf8(ws);
	std::string result2 = utf16_to_utf8(ws.c_str());
	EXPECT_EQ(result1, result2);

	// Test empty wstring
	ws.clear();
	result1 = utf16_to_utf8(ws);
	EXPECT_TRUE(result1.empty());
}