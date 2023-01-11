#pragma once

#include "ParsedHttpRequest.h"

namespace
{
	constexpr char STRING_END{ '\0' };
	constexpr std::string_view EMPTY_STRING{ "" };

	constexpr std::pair<uint8_t, uint8_t> BOUNDARY_CODES_OF_ASCII_TABLE{ 0, 127 };
	constexpr std::pair<uint8_t, uint8_t> FIRST_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE{ 33, 47 };
	constexpr std::pair<uint8_t, uint8_t> SECOND_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE{ 58, 64 };
	constexpr std::pair<uint8_t, uint8_t> THIRD_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE{ 91, 96 };
	constexpr std::pair<uint8_t, uint8_t> FOURTH_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE{ 123, 126 };

	constexpr uint8_t LAST_CONTROL_CHARACTER_OF_ASCII_TABLE{ 31 };

	constexpr uint8_t HYPHEN{ '-' };
	constexpr uint8_t WHITESPACE{ ' ' };
	constexpr uint8_t COLON{ ':' };
	constexpr uint8_t CARRIAGE_RETURN{ '\r' };
	constexpr uint8_t LINE_FEED{ '\n' };
}

class HttpRequestsParser
{
	enum class ParsingState : uint8_t
	{
		httpRequestTypeSearch,
		urlSearch,
		httpVersionSearch,
		httpRequestHeadersSearch,
		successfullyParsed,
		notParsed,
	};

	const std::unordered_map<std::string_view, HttpRequestType> m_knownHttpRequestTypes;
	std::string m_rawHttpRequest;
	ParsedHttpRequest m_parsedHttpRequest;
	ParsingState m_parsingState;

public:
	HttpRequestsParser() noexcept;
	~HttpRequestsParser() = default;

	HttpRequestsParser(const HttpRequestsParser& other) = delete;
	HttpRequestsParser& operator=(const HttpRequestsParser& other) = delete;

	HttpRequestsParser(HttpRequestsParser&& other) noexcept;
	HttpRequestsParser& operator=(HttpRequestsParser&& other) noexcept;

	bool isRequestSuccessfullyParsed();
	HttpRequestType getRequestType();
	std::string_view getRequestUrl();
	std::string_view getHttpVersion();
	std::string_view getValueOfHeaderOfRequest(const std::string_view& keyOfHeaderOfRequest);

	bool parse(std::string&& httpRequest);

private:
	std::string_view getSubstringOfRequest(size_t begin, size_t end);

	bool checkEndOfLineInRequest(size_t stringCharacterIndex);
	bool checkStringIsFromPrintableCharacters(std::string_view stringToTest);

	bool isValidCharacter(char character);
	bool isControlCharacter(char character);
	bool isSpecialCharacter(char character);
};

