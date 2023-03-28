#include "HttpRequest.h"

HttpRequest::HttpRequest()
	: m_rawHttpRequest{}
	, m_httpMethod{ HttpMethods::invalid }
	, m_url{}
	, m_requestHeaders{}
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
{
	
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	m_rawHttpRequest = other.m_rawHttpRequest;
	m_httpMethod = other.m_httpMethod;
	m_url = other.m_url;
	m_requestHeaders = other.m_requestHeaders;

	return *this;
}


HttpRequest::HttpRequest(HttpRequest&& other) noexcept
	: m_rawHttpRequest{ std::move(other.m_rawHttpRequest) }
	, m_httpMethod{ std::exchange(other.m_httpMethod, HttpMethods::invalid) }
	, m_url{ std::exchange(other.m_url, {}) }
	, m_requestHeaders{ std::move(other.m_requestHeaders) }
{
	
}

HttpRequest& HttpRequest::operator=(HttpRequest&& other) noexcept
{
	m_rawHttpRequest = std::move(other.m_rawHttpRequest);
	m_httpMethod = std::exchange(other.m_httpMethod, HttpMethods::invalid);
	m_url = std::exchange(other.m_url, {});
	m_requestHeaders = std::move(other.m_requestHeaders);

	return *this;
}


 std::string_view HttpRequest::getValueOfHeader(std::string&& keyOfHeaderOfRequest) const
{
	if (!m_requestHeaders.empty())
	{
		std::string keyToSearch{ std::move(keyOfHeaderOfRequest) };

		for (size_t index{ 0 }; index < keyToSearch.size(); ++index)
			keyToSearch[index] = std::tolower(keyToSearch.at(index));

		auto valuePointer{ m_requestHeaders.find(keyToSearch) };

		if (valuePointer != m_requestHeaders.end())
			return valuePointer->second;
	}

	return {};
}