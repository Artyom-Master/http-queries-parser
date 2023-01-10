#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

enum class HttpQueryType : uint8_t
{
	UNKNOWN,
	GET,
	POST,
	CONNECT,
	PUT,
	HEAD,
};

struct ParsedHttpQuery
{
	HttpQueryType type{};
	std::string_view url{};
	std::string_view httpVersion{};
	std::unordered_map<std::string_view, std::string_view> queryHeaders{};
};