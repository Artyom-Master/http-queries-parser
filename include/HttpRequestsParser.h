#pragma once

#include "HttpRequest.h"

namespace
{
	const std::unordered_map<std::string_view, HttpMethods> KNOWN_HTTP_METHODS{
	  {"GET", HttpMethods::get}
	, {"POST", HttpMethods::post}
	, {"CONNECT", HttpMethods::connect}
	, {"PUT", HttpMethods::put}
	, {"HEAD", HttpMethods::head}
	};

	constexpr char STRING_END{ '\0' };

	constexpr std::pair<uint8_t, uint8_t> BOUNDARY_CODES_OF_ASCII_TABLE{ 0, 127 };
	constexpr std::pair<uint8_t, uint8_t> BOUNDARIES_OF_UPPER_CASE_LETTERS{ 'A', 'Z' };
	constexpr std::pair<uint8_t, uint8_t> BOUNDARIES_OF_LOWER_CASE_LETTERS{ 'a', 'z' };

	constexpr uint8_t HYPHEN{ '-' };
	constexpr uint8_t WHITESPACE{ ' ' };
	constexpr uint8_t COLON{ ':' };
	constexpr uint8_t CARRIAGE_RETURN{ '\r' };
	constexpr uint8_t LINE_FEED{ '\n' };

	constexpr uint8_t LAST_CONTROL_CHARACTER_OF_ASCII_TABLE{ 31 };
}

class HttpRequestsParser
{
	enum class ParsingState : uint8_t
	{
		httpRequestTypeSearch,
		urlSearch,
		httpVersionSearch,
		httpRequestHeadersSearch,
		idle,
	};

	ParsingState m_parsingState;

public:
	HttpRequestsParser();
	~HttpRequestsParser() = default;

	HttpRequestsParser(const HttpRequestsParser& other) = delete;
	HttpRequestsParser& operator=(const HttpRequestsParser& other) = delete;

	HttpRequestsParser(HttpRequestsParser&& other) noexcept = delete;
	HttpRequestsParser& operator=(HttpRequestsParser&& other) noexcept = delete;

	bool isWorking();
	HttpRequest parse(std::string&& httpRequest);

private:
	std::string_view getSubstringOfRequest(const HttpRequest& request, 
		size_t begin, size_t end);
	bool checkEndOfLineInRequest(const HttpRequest& request, 
		size_t stringCharacterIndex);
	
	bool isValidCharacter(char character);
	bool isControlCharacter(char character);
	bool isCharacterIsLetterOrHyphen(char character);
};

