#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

enum class HttpRequestType : uint8_t
{
	unknown,
	get,
	post,
	connect,
	put,
	head,
};

struct ParsedHttpRequest
{
	HttpRequestType type;
	std::string_view url;
	std::string_view httpVersion;
	std::unordered_map<std::string_view, std::string_view> requestHeaders;

	ParsedHttpRequest() noexcept
		: type{ HttpRequestType::unknown }
		, url{ "" }
		, httpVersion{ "" }
		, requestHeaders{}
	{

	}

	ParsedHttpRequest(const ParsedHttpRequest& other) = delete;
	ParsedHttpRequest& operator=(const ParsedHttpRequest& other) = delete;

	ParsedHttpRequest(ParsedHttpRequest&& other) noexcept
		: type{ std::exchange(other.type, HttpRequestType::unknown) }
		, url{ std::exchange(other.url, "") }
		, httpVersion{ std::exchange(other.httpVersion, "") }
		, requestHeaders{ std::move(other.requestHeaders) }
	{

	}

	ParsedHttpRequest& operator=(ParsedHttpRequest&& other) noexcept
	{
		type = std::exchange(other.type, HttpRequestType::unknown);
		url = std::exchange(other.url, "");
		httpVersion = std::exchange(other.httpVersion, "");
		requestHeaders = std::move(other.requestHeaders);

		return *this;
	}
};