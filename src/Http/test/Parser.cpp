#include <gtest/gtest.h>

#include "../Parser.hpp"

using namespace Server;

TEST(Parser, ConstructorReturnsObjectOnHttp_1_0_GetRequestWithNoHeaders) {
    const auto request = 
        "GET /some/path HTTP/1.0\r\n"
        "\r\n";

    auto parserResult = Http::Parser::create(request);
    auto& parser = parserResult.value();

    EXPECT_EQ(parser.getRequestType(), Http::Parser::RequestType::GET);
    EXPECT_EQ(parser.getPath(), "/some/path"sv);
    EXPECT_EQ(parser.getVersion(), Http::Parser::VersionType::HTTP_1_0);
}

TEST(Parser, ConstructorReturnsObjectOnHttp_1_1_GetRequestWithHostHeader) {
    const auto request = 
        "GET /some/path HTTP/1.1\r\n"
        "Host: testhost.pl:1234\r\n"
        "\r\n";

    auto parserResult = Http::Parser::create(request);
    auto& parser = parserResult.value();

    EXPECT_EQ(parser.getRequestType(), Http::Parser::RequestType::GET);
    EXPECT_EQ(parser.getPath(), "/some/path"sv);
    EXPECT_EQ(parser.getVersion(), Http::Parser::VersionType::HTTP_1_1);
    EXPECT_EQ(parser.mHost.value(), "testhost.pl:1234");
}