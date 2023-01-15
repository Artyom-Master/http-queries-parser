#include "HttpRequestsParser.h"

HttpRequestsParser::HttpRequestsParser()
	: m_parsingState{ ParsingState::idle }
{

}


bool HttpRequestsParser::isWorking() const
{
	if (m_parsingState == ParsingState::idle)
		return false;
	else
		return true;
}

HttpRequest HttpRequestsParser::parse(std::string&& httpRequest)
{
	if (httpRequest.size() > 1)
	{
		HttpRequest parsedRequest;
		parsedRequest.m_rawHttpRequest = std::move(httpRequest);
		size_t startOfCheckedSubstring{ 0 };
		m_parsingState = ParsingState::httpRequestTypeSearch;

		for(size_t endOfCheckedSubstring{ 1 };
			endOfCheckedSubstring < parsedRequest.m_rawHttpRequest.size();
			++endOfCheckedSubstring)
		{
			if (!isValidCharacter(parsedRequest.m_rawHttpRequest.at(endOfCheckedSubstring)))
			{
				m_parsingState = ParsingState::idle;
				return parsedRequest;
			}

			switch (m_parsingState)
			{

			case ParsingState::httpRequestTypeSearch:
			{
				if (isControlCharacter(parsedRequest.m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::idle;
					return parsedRequest;
				}

				if (parsedRequest.m_rawHttpRequest.at(endOfCheckedSubstring) == WHITESPACE)
				{
					std::string_view assumedHttpMethod{ 
						getSubstringOfRequest(parsedRequest, startOfCheckedSubstring, endOfCheckedSubstring) };

					if(KNOWN_HTTP_METHODS.find(assumedHttpMethod)
						== KNOWN_HTTP_METHODS.end())
					{
						m_parsingState = ParsingState::idle;
						return parsedRequest;
					}

					parsedRequest.m_httpMethod = KNOWN_HTTP_METHODS.at(assumedHttpMethod);
					m_parsingState = ParsingState::urlSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 1;
				}
				else
					continue;
			}break;

			case ParsingState::urlSearch:
			{
				if (isControlCharacter(parsedRequest.m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::idle;
					return parsedRequest;
				}

				if (parsedRequest.m_rawHttpRequest.at(endOfCheckedSubstring) == WHITESPACE)
				{
					std::string_view assumedUrl{
						getSubstringOfRequest(parsedRequest, startOfCheckedSubstring, endOfCheckedSubstring) };

					parsedRequest.m_url = assumedUrl;
					m_parsingState = ParsingState::httpVersionSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 1;
				}
				else
					continue;
			}break;

			case ParsingState::httpVersionSearch:
			{
				if(checkEndOfLineInRequest(parsedRequest, endOfCheckedSubstring))
				{
					m_parsingState = ParsingState::httpRequestHeadersSearch;

					startOfCheckedSubstring = endOfCheckedSubstring + 2;
					++endOfCheckedSubstring;
				}
				else if (isControlCharacter(parsedRequest.m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::idle;
					return parsedRequest;
				}
				else
					continue;
			}break;

			case ParsingState::httpRequestHeadersSearch:
			{
				if (checkEndOfLineInRequest(parsedRequest, endOfCheckedSubstring))
				{
					size_t currentHeaderRequestPointer{ startOfCheckedSubstring };

					while (parsedRequest.m_rawHttpRequest.at(currentHeaderRequestPointer) != COLON)
					{
						if (currentHeaderRequestPointer == endOfCheckedSubstring
							|| !isCharacterIsLetterOrHyphen(parsedRequest.m_rawHttpRequest.at(currentHeaderRequestPointer)))
						{
							m_parsingState = ParsingState::idle;
							return parsedRequest;
						}

						parsedRequest.m_rawHttpRequest[currentHeaderRequestPointer] =
							std::tolower(parsedRequest.m_rawHttpRequest.at(currentHeaderRequestPointer));

						++currentHeaderRequestPointer;
					}

					parsedRequest.m_requestHeaders[
						getSubstringOfRequest(parsedRequest, startOfCheckedSubstring, currentHeaderRequestPointer) ]
						= getSubstringOfRequest(parsedRequest, currentHeaderRequestPointer + 2, endOfCheckedSubstring);

					startOfCheckedSubstring = endOfCheckedSubstring + 2;
					++endOfCheckedSubstring;

					if (endOfCheckedSubstring + 2 < parsedRequest.m_rawHttpRequest.size()
						&& checkEndOfLineInRequest(parsedRequest, endOfCheckedSubstring + 1))
					{
						m_parsingState = ParsingState::idle;
						parsedRequest.m_valid = true;
						return parsedRequest;
					}
				}
				else if (isControlCharacter(parsedRequest.m_rawHttpRequest.at(endOfCheckedSubstring)))
				{
					m_parsingState = ParsingState::idle;
					return parsedRequest;
				}
				else
					continue;
			}break;

			}
		}

		m_parsingState = ParsingState::idle;
		parsedRequest.m_valid = true;
		return parsedRequest;
	}
	else
		return {};
}


std::string_view HttpRequestsParser::getSubstringOfRequest(const HttpRequest& request, 
	size_t begin, size_t end)
{
	return std::string_view(request.m_rawHttpRequest).substr(begin, end - begin);
}


bool HttpRequestsParser::checkEndOfLineInRequest(const HttpRequest& request, 
	size_t stringCharacterIndex)
{
	if ((request.m_rawHttpRequest.at(stringCharacterIndex) == CARRIAGE_RETURN
		&& request.m_rawHttpRequest.at(stringCharacterIndex + 1) == LINE_FEED))
		return true;
	else
		return false;
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

bool HttpRequestsParser::isCharacterIsLetterOrHyphen(char character)
{
	return (character >= BOUNDARIES_OF_UPPER_CASE_LETTERS.first
		&& character <= BOUNDARIES_OF_UPPER_CASE_LETTERS.second)

		|| (character >= BOUNDARIES_OF_LOWER_CASE_LETTERS.first
			&& character <= BOUNDARIES_OF_LOWER_CASE_LETTERS.second)

		|| character == HYPHEN;
}