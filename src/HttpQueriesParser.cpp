#include "HttpQueriesParser.h"

HttpQueriesParser::HttpQueriesParser() noexcept
	: m_httpQueryTypesMapForParsing{ 
	  {"GET", HttpQueryType::GET}
	, {"POST", HttpQueryType::POST}
	, {"CONNECT", HttpQueryType::CONNECT}
	, {"PUT", HttpQueryType::PUT}
	, {"HEAD", HttpQueryType::HEAD}
	}
	, m_lastParsedHttpQuery{}
	, m_lastParsedHttpQueryValid{ false }
{

}


HttpQueriesParser::HttpQueriesParser(HttpQueriesParser&& other) noexcept
	: m_httpQueryTypesMapForParsing{ std::move(other.m_httpQueryTypesMapForParsing) }
	, m_lastParsedHttpQuery{ std::move(other.m_lastParsedHttpQuery) }
	, m_lastParsedHttpQueryValid{ std::move(other.m_lastParsedHttpQueryValid) }
{

}

HttpQueriesParser& HttpQueriesParser::operator=(HttpQueriesParser&& other) noexcept
{
	m_httpQueryTypesMapForParsing = std::move(other.m_httpQueryTypesMapForParsing);
	m_lastParsedHttpQuery = std::move(other.m_lastParsedHttpQuery);
	m_lastParsedHttpQueryValid = std::move(other.m_lastParsedHttpQueryValid);

	return *this;
}


bool HttpQueriesParser::isQueryValid()
{
	return m_lastParsedHttpQueryValid;
}

HttpQueryType HttpQueriesParser::getQueryType()
{
	if (m_lastParsedHttpQueryValid)
		return m_lastParsedHttpQuery.type;
	else
		return HttpQueryType::UNKNOWN;
}

std::string_view HttpQueriesParser::getQueryUrl()
{
	if (m_lastParsedHttpQueryValid)
		return m_lastParsedHttpQuery.url;
	else
		return "";
}

std::string_view HttpQueriesParser::getHttpVersion()
{
	if (m_lastParsedHttpQueryValid)
		return m_lastParsedHttpQuery.httpVersion;
	else
		return "";
}

std::string_view HttpQueriesParser::getValueOfHeaderOfQuery(const std::string_view& keyOfHeaderOfQuery)
{
	if (m_lastParsedHttpQueryValid)
		return m_lastParsedHttpQuery.queryHeaders.at(keyOfHeaderOfQuery);
	else
		return "";
}

bool HttpQueriesParser::parse(const char* httpQuery)
{
	m_lastParsedHttpQueryValid = false;
	char stringEnd{ '\0' };

	if (httpQuery != nullptr && *httpQuery != stringEnd)
	{
		const char* substringStartPointer{ httpQuery };
		const char* substringEndPointer{ httpQuery + 1 };
		ParseState parseState{ ParseState::httpQueryTypeSearch };

		while (*substringEndPointer != stringEnd)
		{
			++substringEndPointer;

			switch (parseState)
			{
			case ParseState::httpQueryTypeSearch:
			{
				if (*substringEndPointer == ' ')
				{
					std::string_view assumedHttpQueryType(substringStartPointer
						, substringEndPointer - substringStartPointer);

					m_lastParsedHttpQuery.type = m_httpQueryTypesMapForParsing[assumedHttpQueryType];

					if (m_lastParsedHttpQuery.type == HttpQueryType::UNKNOWN)
						return m_lastParsedHttpQueryValid;

					parseState = ParseState::urlSearch;
				}
				else
					continue;
			}break;
			case ParseState::urlSearch:
			{
				if (*substringEndPointer == ' ')
				{
					m_lastParsedHttpQuery.url = std::move(std::string_view(substringStartPointer
						, substringEndPointer - substringStartPointer));

					for (const auto& character : m_lastParsedHttpQuery.url)
					{
						if (!isValidChar(&character)
							|| isControlChar(&character))
							return m_lastParsedHttpQueryValid;
					}

					parseState = ParseState::httpVersionSearch;
				}
				else
					continue;
			}break;
			case ParseState::httpVersionSearch:
			{
				if ((*substringEndPointer == '\r' && *(substringEndPointer + 1) == '\n')
					|| *substringEndPointer == '\n')
				{
					m_lastParsedHttpQuery.httpVersion = std::move(std::string_view(substringStartPointer
						, substringEndPointer - substringStartPointer));

					for (const auto& character : m_lastParsedHttpQuery.url)
					{
						if (!isValidChar(&character)
							|| isControlChar(&character))
							return m_lastParsedHttpQueryValid;
					}

					parseState = ParseState::httpQueryHeadersSearch;
				}
				else
					continue;
			}break;
			case ParseState::httpQueryHeadersSearch:
			{
				if ((*substringEndPointer == '\r' && *(substringEndPointer + 1) == '\n')
					|| *substringEndPointer == '\n')
				{
					const char* currentQueryHeaderPointer{ substringStartPointer };

					while (*currentQueryHeaderPointer != ':')
					{
						if (*currentQueryHeaderPointer == stringEnd
							|| !isValidChar(currentQueryHeaderPointer)
							|| isControlChar(currentQueryHeaderPointer)
							|| isSpecialChar(currentQueryHeaderPointer))
							return m_lastParsedHttpQueryValid;

						++currentQueryHeaderPointer;
					}

					std::string_view key = std::move(std::string_view(substringStartPointer
						, currentQueryHeaderPointer - substringStartPointer));

					m_lastParsedHttpQuery.queryHeaders[key]
						= std::move(std::string_view(currentQueryHeaderPointer + 2
							, substringEndPointer - currentQueryHeaderPointer - 2));

					for (const auto& character : m_lastParsedHttpQuery.queryHeaders.at(key))
					{
						if (!isValidChar(&character)
							|| isControlChar(&character))
							return m_lastParsedHttpQueryValid;
					}
				}
				else
					continue;
			}break;
			}

			if((*substringEndPointer == '\r' && *(substringEndPointer + 1) == '\n'))
				substringStartPointer = substringEndPointer + 2;
			else
				substringStartPointer = substringEndPointer + 1;

			substringEndPointer = substringStartPointer;
		}

		m_lastParsedHttpQueryValid = true;
	}

	return m_lastParsedHttpQueryValid;
}

bool HttpQueriesParser::isValidChar(const char* character)
{
	return (*character >= 0 && *character <= 127);
}

bool HttpQueriesParser::isControlChar(const char* character)
{
	return (*character >= 0 && *character <= 31) 
		|| *character == 127;
}

bool HttpQueriesParser::isSpecialChar(const char* character)
{
	// return false if character is letter, digit, '-' or control char
	return (*character >= 33 && *character <= 44)
		|| (*character >= 46 && *character <= 47)
		|| (*character >= 58 && *character <= 64)
		|| (*character >= 91 && *character <= 96)
		|| (*character >= 123 && *character <= 126);
}