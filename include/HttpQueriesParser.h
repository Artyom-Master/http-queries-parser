#pragma once

#include "ParsedHttpQuery.h"

class HttpQueriesParser
{
	enum class ParseState
	{
		httpQueryTypeSearch,
		urlSearch,
		httpVersionSearch,
		httpQueryHeadersSearch,
	};

	std::unordered_map<std::string_view, HttpQueryType> m_httpQueryTypesMapForParsing;
	ParsedHttpQuery m_lastParsedHttpQuery;
	bool m_lastParsedHttpQueryValid;

public:
	HttpQueriesParser() noexcept;
	~HttpQueriesParser() = default;

	HttpQueriesParser(const HttpQueriesParser& other) = delete;
	HttpQueriesParser& operator=(const HttpQueriesParser& other) = delete;

	HttpQueriesParser(HttpQueriesParser&& other) noexcept;
	HttpQueriesParser& operator=(HttpQueriesParser&& other) noexcept;

	bool isQueryValid();
	HttpQueryType getQueryType();
	std::string_view getQueryUrl();
	std::string_view getHttpVersion();
	std::string_view getValueOfHeaderOfQuery(const std::string_view& keyOfHeaderOfQuery);

	bool parse(const char* httpQuery);

private:
	bool isValidChar(const char* character);
	bool isControlChar(const char* character);
	bool isSpecialChar(const char* character);
};

