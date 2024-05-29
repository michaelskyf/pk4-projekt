#include "Parser.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <map>
#include <ranges>
#include <stdexcept>
#include <string_view>

#include <iostream>
#include <tuple>
using namespace Server;

namespace {

enum class HeaderType {
    HOST,
    USER_AGENT,
    ACCEPT,
};

const auto headerStrToTypeMap = std::map<std::string_view, HeaderType>{
    {"Host", HeaderType::HOST},
    {"User-Agent", HeaderType::USER_AGENT},
    {"Accept", HeaderType::ACCEPT},
};

const auto headerStrToRequestTypeMap = std::map<std::string_view, Http::Parser::RequestType>{
    {"GET", Http::Parser::RequestType::GET},
};

const auto headerStrToVersionTypeMap = std::map<std::string_view, Http::Parser::VersionType>{
    {"HTTP/1.0", Http::Parser::VersionType::HTTP_1_0},
    {"HTTP/1.1", Http::Parser::VersionType::HTTP_1_1},
};

auto split_header(std::string_view header) -> std::tuple<std::string_view, std::string_view> {
    auto pos = strchr(header.data(), ':');
    if(pos == nullptr) {
        throw std::runtime_error("Expected ':'");
    }

    constexpr auto accept = " \f\n\r\t\v";
    pos += 1;
    auto span = strspn(pos, accept);
    return {{header.begin(), pos-1}, {pos + span, header.end()}};
}

auto parse_http(std::string_view request) -> std::tuple<Http::Parser::RequestType, std::string_view, Http::Parser::VersionType> {
    auto split = std::views::split(request, ' ');
    auto it = split.begin();
    
    auto requestTypeSplit = *it++;
    auto requestType = headerStrToRequestTypeMap.at(std::string_view(requestTypeSplit.begin(), requestTypeSplit.end()));
    
    auto pathSplit = *it++;
    auto path = std::string_view(pathSplit.begin(), pathSplit.size());

    auto versionSplit = *it;
    auto version = headerStrToVersionTypeMap.at(std::string_view(versionSplit.begin(), versionSplit.end()));

    return {requestType, path, version};
}

} // namespace

Http::Parser::Parser(std::string_view request)
    : mOriginalRequest{request} {

    // Validate that the request ends with \r\n\r\n
    if(!request.ends_with("\r\n\r\n")) {
        throw std::runtime_error(R"(Request does not end with \r\n\r\n)");
    }

    constexpr auto delim{"\r\n"sv};
    auto split = std::views::split(request, delim);
    auto it = split.begin();

    auto hdr = *it;
    auto[requestType, path, version] = parse_http({hdr.begin(), hdr.end()});
    mRequestType = requestType;
    mPath = path;
    mVersion = version;

    it++;
    for(; it != split.end(); it++) {
        auto header = *it;
        if(header.size() == 0) continue;
        auto headerView = std::string_view(header.begin(), header.size());
        auto[headerName, headerValue] = split_header(headerView);

        auto typeIt = headerStrToTypeMap.find(headerName);
        if(typeIt == headerStrToTypeMap.end()) continue;

        auto type = typeIt->second;
        switch(type) {
            case HeaderType::HOST: {
                mHost = headerValue;
            } break;

            case HeaderType::ACCEPT: {
                
            } break;

            case HeaderType::USER_AGENT: {
                
            } break;
        }
    }

    // Validate that the required values are set
    if(!mRequestType.has_value() || !mPath.has_value() || !mVersion.has_value()) {
        throw std::runtime_error("Request does not contain type, path or version");
    }

    if(mVersion == Parser::VersionType::HTTP_1_1 && !mHost.has_value()) {
        throw std::runtime_error("Request with HTTP version 1.1 does not contain HOST header");
    }
}