# Description
That's a project for parsing HTTP requests which is wrote on plain C++.<br>
There is *struct* **ParsedHttpRequest** for storing the parse results and *class* **HttpRequestsParser** for parsing.
The most interesting function is **bool HttpRequestsParser::parse(std::string&& httpRequest)**.
The process of parsing consists of next steps:

- clear results of previous request parsing;
- search for HTTP method (printable ASCII characters before first whitespace);
- search for URL (printable ASCII characters before second whitespace);
- skipping HTTP version (printable ASCII characters before \<CR\>\<LF\>);
- search for headers (printable ASCII characters before \<CR\>\<LF\>) each of which consists of:
  - name (ASCII letters or hyphen);
  - hyphen and whitespace;
  - value (printable ASCII characters before \<CR\>\<LF\>).
- search for pure \<CR\>\<LF\> or end of request after previous header.

If during substring of request validation it doesn't meet the certain requirements above, parsing is being stopped and nothing meaningful can be gotten from parser after that. Otherwise after successfull parsing there are some functions to receive useful information about parsed HTTP request:

- HTTP method of request (get, post, put, connect or head) by **HttpMethods HttpRequestsParser::getHttpMethod()**;
- URL of request by **std::string_view HttpRequestsParser::getUrl()**;
- value by its name (case insensitive) in header by **std::string_view HttpRequestsParser::getValueOfHeader(std::string&& keyOfHeaderOfRequest)**;

Also it can be iterated over parsed request headers by the next way:
```
    for (auto iterator{ httpRequestsParser->beginOfHeaders() };
        iterator != httpRequestsParser->endOfHeaders(); ++iterator)
        std::cout << iterator->first << ": " << iterator->second << "\n";
```
To know if request parsed successfully **bool HttpRequestsParser::isRequestSuccessfullyParsed()** should be called.<br>
Test example how to use parser is given in *main.cpp*.<br><br>
Visual Studio 2019 and MSVC v142 were used for project building.
