# Description
That's a project for parsing HTTP requests which is wrote on plain C++.<br>
There is *class* **HttpRequest** for storing the parse results and *class* **HttpRequestsParser** for parsing.
The most interesting method is **bool HttpRequestsParser::parse(std::string&& httpRequest)**.
The process of parsing consists of next steps:

- search for HTTP method (printable ASCII characters before first whitespace);
- search for URL (printable ASCII characters before second whitespace);
- skipping HTTP version (printable ASCII characters before \<CR\>\<LF\>);
- search for headers (printable ASCII characters before \<CR\>\<LF\>) each of which consists of:
  - name (ASCII letters or hyphen);
  - hyphen and whitespace;
  - value (printable ASCII characters before \<CR\>\<LF\>).
- search for pure \<CR\>\<LF\> or end of request after previous header.

If during substring of request validation it doesn't meet the certain requirements above, parsing is being stopped and invalid **HttpRequest** object is returned from method (invalid means that all get methods return void results). Otherwise after successfull parsing there are some methods of **HttpRequest** to receive request data:

- HTTP method of request (get, post, put, connect or head) by **HttpMethods HttpRequest::getHttpMethod()** *method*;
- URL of request by **std::string_view HttpRequest::getUrl()** *method*;
- value by its name (case insensitive) in header by **std::string_view HttpRequest::getValueOfHeader(std::string&& keyOfHeaderOfRequest)** *method*;

Also it can be iterated over parsed request headers by the next way:
```
    for (auto iterator{ httpRequest->beginOfHeaders() };
        iterator != httpRequest->endOfHeaders(); ++iterator)
        std::cout << iterator->first << ": " << iterator->second << "\n";
```
To know if request parsed successfully then **bool HttpRequest::isValid()** *method* should be called.<br>
The parser doesn't recognize HTTP version and data after headers and doesn't provide the way to know it.<br>
Test example how to use parser is given in *main.cpp*.<br><br>
Visual Studio 2022 and MSVC v143 were used for project building.
