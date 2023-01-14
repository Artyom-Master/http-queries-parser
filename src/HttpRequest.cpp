#include "HttpRequest.h"

HttpRequest::HttpRequest()
	: m_rawHttpRequest{}
	, m_httpMethod{ HttpMethods::unknown }
	, m_url{}
	, m_requestHeaders{}
	, m_valid{ false }
{
	
}

HttpRequest::~HttpRequest()
{
	m_requestHeaders.clear();
	m_rawHttpRequest.clear();
	m_rawHttpRequest.shrink_to_fit();
}


HttpRequest::HttpRequest(const HttpRequest& other)
	: m_rawHttpRequest{ other.m_rawHttpRequest }
	, m_httpMethod{ other.m_httpMethod }
	, m_url{ other.m_url }
	, m_requestHeaders{ other.m_requestHeaders }
	, m_valid{ other.m_valid }
{
	
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	m_rawHttpRequest = other.m_rawHttpRequest;
	m_httpMethod = other.m_httpMethod;
	m_url = other.m_url;
	m_requestHeaders = other.m_requestHeaders;
	m_valid = other.m_valid;

	return *this;
}


HttpRequest::HttpRequest(HttpRequest&& other) noexcept
	: m_rawHttpRequest{ std::move(other.m_rawHttpRequest) }
	, m_httpMethod{ std::exchange(other.m_httpMethod, HttpMethods::unknown) }
	, m_url{ std::exchange(other.m_url, {}) }
	, m_requestHeaders{ std::move(other.m_requestHeaders) }
	, m_valid{ std::exchange(other.m_valid, false) }
{
	
}

HttpRequest& HttpRequest::operator=(HttpRequest&& other) noexcept
{
	m_rawHttpRequest = std::move(other.m_rawHttpRequest);
	m_httpMethod = std::exchange(other.m_httpMethod, HttpMethods::unknown);
	m_url = std::exchange(other.m_url, {});
	m_requestHeaders = std::move(other.m_requestHeaders);
	m_valid = std::exchange(other.m_valid, false);

	return *this;
}


bool HttpRequest::isValid()
{
	return m_valid;
}

HttpMethods HttpRequest::getHttpMethod()
{
	if (m_valid)
		return m_httpMethod;
	else
		return HttpMethods::unknown;
}

std::string_view HttpRequest::getUrl()
{
	if (m_valid)
		return m_url;
	else
		return {};
}

std::string_view HttpRequest::getValueOfHeader(std::string&& keyOfHeaderOfRequest)
{
	if (m_valid)
	{
		std::string keyToSearch{ std::move(keyOfHeaderOfRequest) };

		for (int index{ 0 }; index < keyToSearch.size(); ++index)
			keyToSearch[index] = std::tolower(keyToSearch.at(index));

		if (m_requestHeaders.find(keyToSearch)
			!= m_requestHeaders.end())
			return m_requestHeaders.at(keyToSearch);
		else
			return {};
	}
	else
		return {};
}

std::unordered_map<std::string_view, std::string_view>::iterator
HttpRequest::getBeginOfHeaders()
{
	if (m_valid)
		return m_requestHeaders.begin();
	else
		return getEndOfHeaders();
}

std::unordered_map<std::string_view, std::string_view>::iterator
HttpRequest::getEndOfHeaders()
{
	return m_requestHeaders.end();
}