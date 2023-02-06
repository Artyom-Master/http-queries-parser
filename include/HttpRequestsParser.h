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

	constexpr std::pair<char, char> BOUNDARY_CODES_OF_ASCII_TABLE{ 0, 127 };
	constexpr std::pair<char, char> BOUNDARIES_OF_UPPER_CASE_LETTERS{ 'A', 'Z' };
	constexpr std::pair<char, char> BOUNDARIES_OF_LOWER_CASE_LETTERS{ 'a', 'z' };

	constexpr char HYPHEN{ '-' };
	constexpr char WHITESPACE{ ' ' };
	constexpr char COLON{ ':' };
	constexpr char CARRIAGE_RETURN{ '\r' };
	constexpr char LINE_FEED{ '\n' };

	constexpr char LAST_CONTROL_CHARACTER_OF_ASCII_TABLE{ 31 };
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

	inline bool isWorking() const { return m_parsingState != ParsingState::idle; }
	HttpRequest parse(std::string&& httpRequest);

private:
	inline std::string_view getSubstringOfRequest(const HttpRequest& request,
		size_t begin, size_t end) const;
	inline bool checkEndOfLineInRequest(const HttpRequest& request,
		size_t stringCharacterIndex) const;
	
	inline bool isValidCharacter(char character) const;
	inline bool isControlCharacter(char character) const;
	inline bool isCharacterIsLetterOrHyphen(char character) const;
};

