#pragma once

#include "ParsedHttpRequest.h"

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
	HttpMethods getHttpMethod();
	std::string_view getUrl();
	std::string_view getValueOfHeader(std::string&& keyOfHeaderOfRequest);

	bool parse(std::string&& httpRequest);

private:
	std::string_view getSubstringOfRequest(size_t begin, size_t end);

	bool checkEndOfLineInRequest(size_t stringCharacterIndex);
	char lowCharacterCaseInRequest(char character);

	bool isValidCharacter(char character);
	bool isControlCharacter(char character);
	bool isSpecialCharacter(char character);
};

