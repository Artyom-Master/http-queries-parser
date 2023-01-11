#include "HttpRequestsParser.h"

HttpRequestsParser::HttpRequestsParser() noexcept
	: m_knownHttpRequestTypes{ 
	  {"GET", HttpRequestType::get}
	, {"POST", HttpRequestType::post}
	, {"CONNECT", HttpRequestType::connect}
	, {"PUT", HttpRequestType::put}
	, {"HEAD", HttpRequestType::head}
	}
	, m_rawHttpRequest{}
	, m_parsedHttpRequest{}
	, m_parsingState{ ParsingState::notParsed }
{

}


HttpRequestsParser::HttpRequestsParser(HttpRequestsParser&& other) noexcept
	: m_rawHttpRequest{ std::move(other.m_rawHttpRequest) }
	, m_parsedHttpRequest{ std::move(other.m_parsedHttpRequest) }
	, m_parsingState{ std::exchange(other.m_parsingState, ParsingState::notParsed) }
{

}

HttpRequestsParser& HttpRequestsParser::operator=(HttpRequestsParser&& other) noexcept
{
	m_rawHttpRequest = std::move(other.m_rawHttpRequest);
	m_parsedHttpRequest = std::move(other.m_parsedHttpRequest);
	m_parsingState = std::exchange(other.m_parsingState, ParsingState::notParsed);

	return *this;
}


bool HttpRequestsParser::isRequestSuccessfullyParsed()
{
	if (m_parsingState == ParsingState::successfullyParsed)
		return true;
	else
		return false;
}

HttpRequestType HttpRequestsParser::getRequestType()
{
	if (isRequestSuccessfullyParsed())
		return m_parsedHttpRequest.type;
	else
		return HttpRequestType::unknown;
}

std::string_view HttpRequestsParser::getRequestUrl()
{
	if (isRequestSuccessfullyParsed())
		return m_parsedHttpRequest.url;
	else
		return EMPTY_STRING;
}

std::string_view HttpRequestsParser::getHttpVersion()
{
	if (isRequestSuccessfullyParsed())
		return m_parsedHttpRequest.httpVersion;
	else
		return EMPTY_STRING;
}

std::string_view HttpRequestsParser::getValueOfHeaderOfRequest(const std::string_view& keyOfHeaderOfRequest)
{
	if (isRequestSuccessfullyParsed())
		if(m_parsedHttpRequest.requestHeaders.find(keyOfHeaderOfRequest)
			!= m_parsedHttpRequest.requestHeaders.end())
		return m_parsedHttpRequest.requestHeaders.at(keyOfHeaderOfRequest);

	return EMPTY_STRING;
}


bool HttpRequestsParser::parse(std::string&& httpRequest)
{
	bool isSuccessfullyParsed{ false };

	if (httpRequest.size() > 1)
	{
		m_rawHttpRequest = std::move(httpRequest);
		size_t startOfCheckedSubstring{ 0 };
		m_parsingState = ParsingState::httpRequestTypeSearch;

		for(size_t endOfCheckedSubstring{ 1 };
			endOfCheckedSubstring < m_rawHttpRequest.size();
			++endOfCheckedSubstring)
		{
			switch (m_parsingState)
			{

			case ParsingState::httpRequestTypeSearch:
			{
				if (m_rawHttpRequest.at(endOfCheckedSubstring) == WHITESPACE)
				{
					std::string_view assumedHttpRequestType{ 
						getSubstringOfRequest(startOfCheckedSubstring, endOfCheckedSubstring) };

					if(m_knownHttpRequestTypes.find(assumedHttpRequestType) 
						== m_knownHttpRequestTypes.end())
					{
						m_parsingState = ParsingState::notParsed;
						return isSuccessfullyParsed;
					}

					m_parsedHttpRequest.type = m_knownHttpRequestTypes.at(assumedHttpRequestType);
					m_parsingState = ParsingState::urlSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 1;
				}
				else
					continue;
			}break;

			case ParsingState::urlSearch:
			{
				if (m_rawHttpRequest.at(endOfCheckedSubstring) == WHITESPACE)
				{
					std::string_view assumedUrl{
						getSubstringOfRequest(startOfCheckedSubstring, endOfCheckedSubstring) };

					if (!checkStringIsFromPrintableCharacters(assumedUrl))
					{
						m_parsingState = ParsingState::notParsed;
						return isSuccessfullyParsed;
					}

					m_parsedHttpRequest.url = assumedUrl;
					m_parsingState = ParsingState::httpVersionSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 1;
				}
				else
					continue;
			}break;

			case ParsingState::httpVersionSearch:
			{
				if(checkEndOfLineInRequest(endOfCheckedSubstring))
				{
					std::string_view assumedHttpVersion{
						getSubstringOfRequest(startOfCheckedSubstring, endOfCheckedSubstring) };

					if (!checkStringIsFromPrintableCharacters(assumedHttpVersion))
					{
						m_parsingState = ParsingState::notParsed;
						return isSuccessfullyParsed;
					}

					m_parsedHttpRequest.httpVersion = assumedHttpVersion;
					m_parsingState = ParsingState::httpRequestHeadersSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 2;
					++endOfCheckedSubstring;
				}
				else
					continue;
			}break;

			case ParsingState::httpRequestHeadersSearch:
			{
				if (checkEndOfLineInRequest(endOfCheckedSubstring))
				{
					size_t currentHeaderRequestPointer{ startOfCheckedSubstring };

					while (m_rawHttpRequest.at(currentHeaderRequestPointer) != COLON)
					{
						if (currentHeaderRequestPointer == endOfCheckedSubstring
							|| !isValidCharacter(m_rawHttpRequest.at(currentHeaderRequestPointer))
							|| isControlCharacter(m_rawHttpRequest.at(currentHeaderRequestPointer))
							|| isSpecialCharacter(m_rawHttpRequest.at(currentHeaderRequestPointer)))
						{
							m_parsingState = ParsingState::notParsed;
							return isSuccessfullyParsed;
						}

						++currentHeaderRequestPointer;
					}

					std::string_view headerValue{
						getSubstringOfRequest(currentHeaderRequestPointer + 2, endOfCheckedSubstring) };

					if (!checkStringIsFromPrintableCharacters(headerValue))
					{
						m_parsingState = ParsingState::notParsed;
						return isSuccessfullyParsed;
					}

					m_parsedHttpRequest.requestHeaders[
						getSubstringOfRequest(startOfCheckedSubstring, currentHeaderRequestPointer) ]
						= headerValue;

					startOfCheckedSubstring = endOfCheckedSubstring + 2;
					++endOfCheckedSubstring;
				}
				else
					continue;
			}break;

			}
		}

		m_parsingState = ParsingState::successfullyParsed;
		isSuccessfullyParsed = true;
	}

	return isSuccessfullyParsed;
}

std::string_view HttpRequestsParser::getSubstringOfRequest(size_t begin, size_t end)
{
	return std::string_view(m_rawHttpRequest).substr(begin, end - begin);
}


bool HttpRequestsParser::checkEndOfLineInRequest(size_t stringCharacterIndex)
{
	if ((m_rawHttpRequest.at(stringCharacterIndex) == CARRIAGE_RETURN
		&& m_rawHttpRequest.at(stringCharacterIndex + 1) == LINE_FEED))
		return true;
	else
		return false;
}

bool HttpRequestsParser::checkStringIsFromPrintableCharacters(std::string_view stringToTest)
{
	for (const auto& character : stringToTest)
	{
		if (!isValidCharacter(character)
			|| isControlCharacter(character))
			return false;
	}

	return true;
}


bool HttpRequestsParser::isValidCharacter(char character)
{
	return (character >= BOUNDARY_CODES_OF_ASCII_TABLE.first
		&& character <= BOUNDARY_CODES_OF_ASCII_TABLE.second);
}

bool HttpRequestsParser::isControlCharacter(char character)
{
	return (character >= BOUNDARY_CODES_OF_ASCII_TABLE.first
		&& character <= LAST_CONTROL_CHARACTER_OF_ASCII_TABLE)
		
		|| character == BOUNDARY_CODES_OF_ASCII_TABLE.second;
}

bool HttpRequestsParser::isSpecialCharacter(char character)
{
	return (character >= FIRST_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.first 
		&& character <= FIRST_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.second
		&& character != HYPHEN)

		|| (character >= SECOND_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.first 
			&& character <= SECOND_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.second)
		
		|| (character >= THIRD_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.first 
			&& character <= THIRD_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.second)
		
		|| (character >= FOURTH_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.first 
			&& character <= FOURTH_PART_OF_SPECIAL_CHARACTERS_OF_ASCII_TABLE.second);
}