#include "ResponseGenerator.hpp"
#include <boost/asio/buffer.hpp>
#include <cstring>
#include <fstream>
#include <string_view>

using namespace Server;

namespace {

auto generate_404() -> std::string {
    return "HTTP/1.1 404 Not Found\r\n\r\n";
}

auto generate_200(size_t fileSize) -> std::string {
    return
    "HTTP/1.1 200\r\n"
    "Content-Length: " + std::to_string(fileSize) + "\r\n"
    "\r\n";  
}

auto read_file(std::ifstream& file, asio::mutable_buffer out) -> Result<Option<size_t>> {
    auto readBytes = file.readsome(reinterpret_cast<char*>(out.data()), static_cast<long>(out.size()));
    if(readBytes == 0) {
        if(file.fail()) return Err(strerror(errno));

        return None;
    }

    return readBytes;
}

} // namespace

auto Http::ResponseGenerator::create(const Parser&& parser, const std::filesystem::directory_entry&& entry) -> Result<ResponseGenerator> {
    auto file = std::ifstream(entry.path(), std::ios::binary);
    if(file) {
        return ResponseGenerator(std::move(parser), std::move(file));
    } else {
        return ResponseGenerator(std::move(parser), None);
    }
}

auto Http::ResponseGenerator::generate(asio::mutable_buffer buffer) -> Result<Option<size_t>> {
    if(mState == State::RESPONSE) {
        if(mFile.has_value() == false) {
            mResponse = generate_404();
        } else {
            mFile->seekg(0, std::ios::end);
            mResponse = generate_200(static_cast<size_t>(mFile->tellg()));
            mFile->seekg(0, std::ios::beg);
        }
    }

    switch(mState) {
        
        case State::RESPONSE: {
            auto dataLeft = mResponse.size() - mResponseOffset;
            if(dataLeft == 0) {
                if(mFile.has_value()) {
                    mState = State::FILE;
                    return generate(buffer);
                } else {
                    return None;
                }
            }
            auto toWrite = buffer.size() < dataLeft ? buffer.size() : dataLeft;

            memcpy(buffer.data(), &mResponse[mResponseOffset], toWrite);

            mResponseOffset += toWrite;
            return toWrite;
        } break;

        case State::FILE: {
            auto result = read_file(mFile.value(), buffer);
            if(result.has_error()) return result;
            if(result.value().has_value() == false) {
                mState = State::RESPONSE;
                return None;
            }

            return result;
        } break;
    }

    return None;
}