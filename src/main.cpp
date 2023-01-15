#include <iostream>

#include "HttpRequestsParser.h"

void printParsedHttpRequest(const HttpRequest& httpRequest)
{
    std::cout << "HTTP method of request: ";

    switch (httpRequest.getHttpMethod())
    {
    case HttpMethods::get:
    {
        std::cout << "GET\n";
    }break;
    case HttpMethods::put:
    {
        std::cout << "PUT\n";
    }break;
    case HttpMethods::connect:
    {
        std::cout << "CONNECT\n";
    }break;
    case HttpMethods::head:
    {
        std::cout << "HEAD\n";
    }break;
    case HttpMethods::post:
    {
        std::cout << "POST\n";
    }break;
    case HttpMethods::unknown:
    {
        std::cout << "UNKNOWN\n";
    }break;
    }

    std::cout << "URL: " << httpRequest.getUrl() << "\n";
    std::cout << "\nEnumeration of headers of request:\n";

    for (auto iterator{ httpRequest.getBeginOfHeaders() };
        iterator != httpRequest.getEndOfHeaders(); ++iterator)
        std::cout << iterator->first << ": " << iterator->second << "\n";

    std::cout << "\nGetting values of headers of request by header name:\n";
    std::cout << "User-Agent: " << httpRequest.getValueOfHeader("User-Agent") << "\n";
    std::cout << "ACCEPT: " << httpRequest.getValueOfHeader("ACCEPT") << "\n";
    std::cout << "connection: " << httpRequest.getValueOfHeader("connection") << "\n";

    std::cout << "\n";
}

int main()
{
    std::unique_ptr<HttpRequestsParser> httpRequestsParser{
        std::make_unique<HttpRequestsParser>() };

    const char* firstRequest{ "GET /wiki/http HTTP/1.1\r\n"
            "Host: ru.wikipedia.org\r\n"
            "User-Agent: Mozilla/5.0 "
            "(X11; U; Linux i686; ru; rv:1.9b5) "
            "Gecko/2008050509 Firefox/3.0b5\r\n"
            "Accept: text/html\r\n"
            "Connection: close\r\n" };
    
    auto firstParsedRequest{ httpRequestsParser->parse(firstRequest) };
        
    if (firstParsedRequest.isValid())
         printParsedHttpRequest(firstParsedRequest);
    else
        std::cout << "Parsing failed, there is syntax error in HTTP request!\n";


    std::cout << "\n\n\n";


    std::string secondRequest{ "POST /path/to/sit(@&*%9jvcsal=sdv=s asfgasxcv&(#@$%H9j9\r\n"
            "ACCEPT text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "some data oajgop$&(**&mv\n\t\r\b" };

    auto secondParsedRequest{ httpRequestsParser->parse(std::move(secondRequest)) };

    if (secondParsedRequest.isValid())
        printParsedHttpRequest(secondParsedRequest);
    else
        std::cout << "Parsing failed, there is syntax error in HTTP request!\n";


    std::cout << "\n\n\n";


	return 0;
}