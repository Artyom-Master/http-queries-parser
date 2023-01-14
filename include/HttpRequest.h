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

class HttpRequest
{
	std::string m_rawHttpRequest;
	HttpMethods m_httpMethod;
	std::string_view m_url;
	std::unordered_map<std::string_view, std::string_view> m_requestHeaders;
	bool m_valid;

public:
	HttpRequest();
	~HttpRequest();

	HttpRequest(const HttpRequest& other);
	HttpRequest& operator=(const HttpRequest& other);

	HttpRequest(HttpRequest&& other) noexcept;
	HttpRequest& operator=(HttpRequest&& other) noexcept;

	bool isValid();
	HttpMethods getHttpMethod();
	std::string_view getUrl();
	std::string_view getValueOfHeader(std::string&& keyOfHeaderOfRequest);

	std::unordered_map<std::string_view, std::string_view>::iterator getBeginOfHeaders();
	std::unordered_map<std::string_view, std::string_view>::iterator getEndOfHeaders();

	friend class HttpRequestsParser;
};