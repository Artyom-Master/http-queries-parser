#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

enum class HttpMethods : uint8_t
{
	unknown,
	get,
	post,
	connect,
	put,
	head,
};

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
	constexpr std::string_view EMPTY_STRING{ "" };

	constexpr std::pair<uint8_t, uint8_t> BOUNDARY_CODES_OF_ASCII_TABLE{ 0, 127 };
	constexpr std::pair<uint8_t, uint8_t> BOUNDARIES_OF_UPPER_CASE_LETTERS{ 'A', 'Z' };
	constexpr std::pair<uint8_t, uint8_t> BOUNDARIES_OF_LOWER_CASE_LETTERS{ 'a', 'z' };

	constexpr uint8_t HYPHEN{ '-' };
	constexpr uint8_t WHITESPACE{ ' ' };
	constexpr uint8_t COLON{ ':' };
	constexpr uint8_t CARRIAGE_RETURN{ '\r' };
	constexpr uint8_t LINE_FEED{ '\n' };

	constexpr uint8_t LAST_CONTROL_CHARACTER_OF_ASCII_TABLE{ 31 };
	constexpr uint8_t UPPER_TO_LOWER_CASE_SHIFT{ 32 };
}

struct ParsedHttpRequest
{
	HttpMethods httpMethod;
	std::string_view url;
	std::unordered_map<std::string_view, std::string_view> requestHeaders;

	ParsedHttpRequest() noexcept
		: httpMethod{ HttpMethods::unknown }
		, url{ EMPTY_STRING }
		, requestHeaders{}
	{

	}

	~ParsedHttpRequest() = default;

	ParsedHttpRequest(const ParsedHttpRequest& other) = delete;
	ParsedHttpRequest& operator=(const ParsedHttpRequest& other) = delete;

	ParsedHttpRequest(ParsedHttpRequest&& other) noexcept
		: httpMethod{ std::exchange(other.httpMethod, HttpMethods::unknown) }
		, url{ std::exchange(other.url, EMPTY_STRING) }
		, requestHeaders{ std::move(other.requestHeaders) }
	{

	}

	ParsedHttpRequest& operator=(ParsedHttpRequest&& other) noexcept
	{
		httpMethod = std::exchange(other.httpMethod, HttpMethods::unknown);
		url = std::exchange(other.url, EMPTY_STRING);
		requestHeaders = std::move(other.requestHeaders);

		return *this;
	}

	void clear()
	{
		httpMethod = HttpMethods::unknown;
		url = EMPTY_STRING;
		requestHeaders.clear();
	}
};