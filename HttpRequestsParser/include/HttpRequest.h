#pragma once

#include <string>
#include <unordered_map>

enum class HttpMethods : uint8_t
{
	invalid,
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

public:
	HttpRequest();
	~HttpRequest();

	HttpRequest(const HttpRequest& other);
	HttpRequest& operator=(const HttpRequest& other);

	HttpRequest(HttpRequest&& other) noexcept;
	HttpRequest& operator=(HttpRequest&& other) noexcept;

	inline bool isValid() const { return m_httpMethod != HttpMethods::invalid; }
	inline HttpMethods getHttpMethod() const { return m_httpMethod; }
	inline std::string_view getUrl() const { return m_url; }
	std::string_view getValueOfHeader(std::string&& keyOfHeaderOfRequest) const;

	inline auto getBeginOfHeaders() const { return m_requestHeaders.begin(); }
	inline auto	getEndOfHeaders() const { return m_requestHeaders.end(); }

	friend class HttpRequestsParser;
};