#include "pch.h"
#include "../HttpRequestsParser/include/HttpRequestsParser.h"

char* realRequest{ "GET /wiki/http HTTP/1.1\r\n"
			"Host: ru.wikipedia.org\r\n"
			"User-Agent: Mozilla/5.0 "
			"(X11; U; Linux i686; ru; rv:1.9b5) "
			"Gecko/2008050509 Firefox/3.0b5\r\n"
			"Accept: text/html\r\n"
			"Connection: close\r\n" };
TEST(TestParseResult, EmptyInput) {
	HttpRequestsParser parser;
	std::string emptyString("");

	EXPECT_FALSE(parser.parse(std::move(emptyString)).isValid());
}

TEST(TestParseResult, InvalidInput) {
	HttpRequestsParser parser;

	EXPECT_FALSE(parser.parse("get").isValid());
	EXPECT_FALSE(parser.parse("DELETE").isValid());
	EXPECT_FALSE(parser.parse("agrew").isValid());
	EXPECT_FALSE(parser.parse("ge").isValid());
}

TEST(TestParseResult, ValidHttpMethodInput) {
	HttpRequestsParser parser;

	for(const auto& httpMethod : KNOWN_HTTP_METHODS)
		EXPECT_TRUE(parser.parse(std::string(httpMethod.first)).isValid());

	EXPECT_TRUE(parser.parse("GET ").isValid());
	EXPECT_TRUE(parser.parse("GET     ").isValid());
	EXPECT_TRUE(parser.parse(realRequest).isValid());
}