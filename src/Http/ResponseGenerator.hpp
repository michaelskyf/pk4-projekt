#pragma once

#include "Common/Common.hpp"
#include "Http/Parser.hpp"
#include <boost/asio/buffer.hpp>
#include <filesystem>
#include <fstream>

namespace Server::Http {

class ResponseGenerator {

    enum class State {
        FILE,
        RESPONSE
    };

    Parser mParser;
    Option<std::ifstream> mFile;
    State mState = State::RESPONSE;
    std::string mResponse;
    size_t mResponseOffset = 0;

    ResponseGenerator(const Parser&& parser, Option<std::ifstream>&& file)
        : mParser{std::move(parser)}, mFile{std::move(file)} {

    }

public:

    static auto create(const Parser&& parser, const std::filesystem::directory_entry&& entry) -> Result<ResponseGenerator>;

    auto generate(asio::mutable_buffer buffer) -> Result<Option<size_t>>;

};

} // namespace Server::Http