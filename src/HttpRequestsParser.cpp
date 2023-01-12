#include "HttpRequestsParser.h"

HttpRequestsParser::HttpRequestsParser() noexcept
	: m_rawHttpRequest{}
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

HttpMethods HttpRequestsParser::getHttpMethod()
{
	if (isRequestSuccessfullyParsed())
		return m_parsedHttpRequest.httpMethod;
	else
		return HttpMethods::unknown;
}

std::string_view HttpRequestsParser::getUrl()
{
	if (isRequestSuccessfullyParsed())
		return m_parsedHttpRequest.url;
	else
		return EMPTY_STRING;
}

std::string_view HttpRequestsParser::getValueOfHeader(std::string&& keyOfHeaderOfRequest)
{
	if (isRequestSuccessfullyParsed())
	{
		std::string keyToSearch{ std::move(keyOfHeaderOfRequest) };

		for (int index{ 0 }; index < keyToSearch.size(); ++index)
			keyToSearch[index] = lowCharacterCaseInRequest(keyToSearch.at(index));

		if (m_parsedHttpRequest.requestHeaders.find(keyToSearch)
			!= m_parsedHttpRequest.requestHeaders.end())
			return m_parsedHttpRequest.requestHeaders.at(keyToSearch);
	}

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
			if (!isValidCharacter(m_rawHttpRequest.at(endOfCheckedSubstring)))
			{
				m_parsingState = ParsingState::notParsed;
				return isSuccessfullyParsed;
			}

			switch (m_parsingState)
			{

			case ParsingState::httpRequestTypeSearch:
			{
				if (isControlCharacter(m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::notParsed;
					return isSuccessfullyParsed;
				}

				if (m_rawHttpRequest.at(endOfCheckedSubstring) == WHITESPACE)
				{
					std::string_view assumedHttpMethod{ 
						getSubstringOfRequest(startOfCheckedSubstring, endOfCheckedSubstring) };

					if(KNOWN_HTTP_METHODS.find(assumedHttpMethod)
						== KNOWN_HTTP_METHODS.end())
					{
						m_parsingState = ParsingState::notParsed;
						return isSuccessfullyParsed;
					}

					m_parsedHttpRequest.httpMethod = KNOWN_HTTP_METHODS.at(assumedHttpMethod);
					m_parsingState = ParsingState::urlSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 1;
				}
				else
					continue;
			}break;

			case ParsingState::urlSearch:
			{
				if (isControlCharacter(m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::notParsed;
					return isSuccessfullyParsed;
				}

				if (m_rawHttpRequest.at(endOfCheckedSubstring) == WHITESPACE)
				{
					std::string_view assumedUrl{
						getSubstringOfRequest(startOfCheckedSubstring, endOfCheckedSubstring) };

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
					m_parsingState = ParsingState::httpRequestHeadersSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 2;
					++endOfCheckedSubstring;
				}
				else if (isControlCharacter(m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::notParsed;
					return isSuccessfullyParsed;
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
							|| isSpecialCharacter(m_rawHttpRequest.at(currentHeaderRequestPointer)))
						{
							m_parsingState = ParsingState::notParsed;
							return isSuccessfullyParsed;
						}

						m_rawHttpRequest[currentHeaderRequestPointer] =
							lowCharacterCaseInRequest(m_rawHttpRequest.at(currentHeaderRequestPointer));

						++currentHeaderRequestPointer;
					}

					m_parsedHttpRequest.requestHeaders[
						getSubstringOfRequest(startOfCheckedSubstring, currentHeaderRequestPointer) ]
						= getSubstringOfRequest(currentHeaderRequestPointer + 2, endOfCheckedSubstring);

					startOfCheckedSubstring = endOfCheckedSubstring + 2;
					++endOfCheckedSubstring;
				}
				else if (isControlCharacter(m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::notParsed;
					return isSuccessfullyParsed;
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

char HttpRequestsParser::lowCharacterCaseInRequest(char character) {
	if (character >= UPPER_A_CHARACTER
		&& character <= UPPER_Z_CHARACTER)
		return character + UPPER_TO_LOWER_CASE_SHIFT;
	else
		return character;
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