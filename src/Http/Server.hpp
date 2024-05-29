#pragma once

#include "Http/ResponseGenerator.hpp"
#include <filesystem>
#include <Common/Common.hpp>
#include <string_view>

namespace Server::Http {

class Server {

    const std::filesystem::directory_entry mRootDirectory;
    
public:

    Server() = delete;

    Server(std::filesystem::directory_entry&& rootDirectory);

    auto parse(std::string_view request) const -> Result<ResponseGenerator>;
};

} // namespace Server::Http 