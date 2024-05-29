#pragma once

#include <Common/Common.hpp>
#include <string_view>

namespace Server::Http {

class Parser {
public:

    enum class RequestType {
        GET,
        POST,
    };

    enum class VersionType {
        HTTP_1_0,
        HTTP_1_1,
    };

private:

    using StringType = std::string_view;
    template <typename T> using Opt = Option<T>;

    Parser(std::string_view request);

    StringType mOriginalRequest;
    // Mandatory fields
    Opt<RequestType> mRequestType;
    Opt<StringType> mPath;
    Opt<VersionType> mVersion;
    // Container for unknown fields
    std::vector<std::pair<StringType, StringType>> mOther;

public:

    static auto create(std::string_view request) -> Result<Parser> {
        try {
            return Parser(request);
        } catch(const std::exception& err) {
            return Err(err.what());
        }
    }

    auto getOriginalRequest() const {
        return mOriginalRequest;
    }

    auto getRequestType() const {
        return mRequestType.value();
    }

    auto getPath() const {
        return mPath.value();
    }

    auto getVersion() const {
        return mVersion.value();
    }

    // Optional fields
    Opt<StringType> mHost;
    Opt<StringType> mUserAgent;
};

} // namespace Server::Http