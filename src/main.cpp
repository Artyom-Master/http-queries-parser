#include <iostream>

#include "HttpQueriesParser.h"

void printParsedHttpQuery(const std::unique_ptr<HttpQueriesParser>& httpQueriesParser)
{
    std::cout << "Type of HTTP query: ";

    switch (httpQueriesParser->getQueryType())
    {
    case HttpQueryType::GET:
    {
        std::cout << "GET\n";
    }break;
    case HttpQueryType::PUT:
    {
        std::cout << "PUT\n";
    }break;
    case HttpQueryType::CONNECT:
    {
        std::cout << "CONNECT\n";
    }break;
    case HttpQueryType::HEAD:
    {
        std::cout << "HEAD\n";
    }break;
    case HttpQueryType::POST:
    {
        std::cout << "POST\n";
    }break;
    case HttpQueryType::UNKNOWN:
    {
        std::cout << "UNKNOWN\n";
    }break;
    }

    std::cout << "URL: " << httpQueriesParser->getQueryUrl() << "\n";
    std::cout << "HTTP version: " << httpQueriesParser->getHttpVersion() << "\n";
    std::cout << "Host: " << httpQueriesParser->getValueOfHeaderOfQuery("Host") << "\n";
    std::cout << "User-Agent: " << httpQueriesParser->getValueOfHeaderOfQuery("User-Agent") << "\n";
    std::cout << "Accept: " << httpQueriesParser->getValueOfHeaderOfQuery("Accept") << "\n";
    std::cout << "Connection: " << httpQueriesParser->getValueOfHeaderOfQuery("Connection") << "\n";
    std::cout << "\n";
}

int main()
{
    std::unique_ptr<HttpQueriesParser> httpQueriesParser{ 
        std::make_unique<HttpQueriesParser>() };

    printParsedHttpQuery(httpQueriesParser);
    std::cout << "\n";

	const char* httpQuery = "GET /wiki/http HTTP/1.1\r\n"
                            "Host: ru.wikipedia.org\r\n"
                            "User-Agent: Mozilla/5.0 "
                            "(X11; U; Linux i686; ru; rv:1.9b5) "
                            "Gecko/2008050509 Firefox/3.0b5\r\n"
                            "Accept: text/html\r\n"
                            "Connection: close\r\n";

    if (httpQueriesParser->parse(httpQuery))
        printParsedHttpQuery(httpQueriesParser);

	return 0;
}