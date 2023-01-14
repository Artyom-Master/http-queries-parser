#include "HttpRequest.h"
#include <iostream>

HttpRequest::HttpRequest()
	: m_rawHttpRequest{}
	, m_httpMethod{ HttpMethods::unknown }
	, m_url{}
	, m_requestHeaders{}
	, m_valid{ false }
{
	std::cout << "\nCall HttpRequest default constructor\n";
}

HttpRequest::~HttpRequest()
{
	m_requestHeaders.clear();
	m_rawHttpRequest.clear();
	m_rawHttpRequest.shrink_to_fit();

	std::cout << "\nCall HttpRequest destructor\n";
}


HttpRequest::HttpRequest(const HttpRequest& other)
	: m_rawHttpRequest{ other.m_rawHttpRequest }
	, m_httpMethod{ other.m_httpMethod }
	, m_url{ other.m_url }
	, m_requestHeaders{ other.m_requestHeaders }
	, m_valid{ other.m_valid }
{
	std::cout << "\nCall HttpRequest copy constructor\n";
}

HttpRequest& HttpRequest::operator=(const HttpRequest& other)
{
	m_rawHttpRequest = other.m_rawHttpRequest;
	m_httpMethod = other.m_httpMethod;
	m_url = other.m_url;
	m_requestHeaders = other.m_requestHeaders;
	m_valid = other.m_valid;

	std::cout << "\nCall HttpRequest copy operator\n";

	return *this;
}


HttpRequest::HttpRequest(HttpRequest&& other) noexcept
	: m_rawHttpRequest{ std::move(other.m_rawHttpRequest) }
	, m_httpMethod{ std::exchange(other.m_httpMethod, HttpMethods::unknown) }
	, m_url{ std::exchange(other.m_url, {}) }
	, m_requestHeaders{ std::move(other.m_requestHeaders) }
	, m_valid{ std::exchange(other.m_valid, false) }
{
	std::cout << "\Call HttpRequest move constructor\n";
}

HttpRequest& HttpRequest::operator=(HttpRequest&& other) noexcept
{
	m_rawHttpRequest = std::move(other.m_rawHttpRequest);
	m_httpMethod = std::exchange(other.m_httpMethod, HttpMethods::unknown);
	m_url = std::exchange(other.m_url, {});
	m_requestHeaders = std::move(other.m_requestHeaders);
	m_valid = std::exchange(other.m_valid, false);

	std::cout << "\Call HttpRequest move operator\n";

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