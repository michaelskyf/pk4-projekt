#include "Server.hpp"
#include "Parser.hpp"
#include "ResponseGenerator.hpp"
#include <Logger/Logger.hpp>

#include <filesystem>

using namespace Server;

Http::Server::Server(std::filesystem::directory_entry&& rootDirectory)
    : mRootDirectory{rootDirectory} {

}

auto Http::Server::parse(std::string_view request) const -> Result<ResponseGenerator> {
    auto parserResult = Parser::create(request);
    if(parserResult.has_error()) return parserResult.error();

    auto& parser = parserResult.value();

    std::filesystem::path path = mRootDirectory.path().string().append(parser.getPath());
    if(std::filesystem::is_directory(path)) {
        path /= "index.html";
    }

    Log("Server: Access to '%s'", path.c_str());
    auto entry = std::filesystem::directory_entry(path);
    return ResponseGenerator::create(std::move(parser), std::move(entry));
}