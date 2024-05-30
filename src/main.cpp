
#include <Http/Server.hpp>
#include "Common/MutexGuard.hpp"
#include "Http/ResponseGenerator.hpp"
#include "Network/IStream.hpp"
#include "Network/Tcp/TcpStream.hpp"
#include <Network/Tcp/TcpAcceptor.hpp>
#include <array>
#include <atomic>
#include <boost/asio/buffer.hpp>
#include <exception>
#include <fstream>
#include <string_view>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/strand.hpp>
#include <filesystem>
#include <functional>
#include <argparse/argparse.hpp>
#include <boost/thread.hpp>

using namespace std::literals;

constexpr auto SERVER_VERSION = "1.0";

using namespace Server;
using namespace Server::Memory;
using namespace Server::Network;
using namespace boost::asio;

using HttpServer = Http::Server;
using HttpResponseGenerator = Http::ResponseGenerator;

namespace {

MutexGuard<size_t> activeConnections = 0;
std::atomic<size_t> requestsServed = 0;

class ConnectionGuard {
    MutexGuard<size_t>& mGuard;
public:
    ConnectionGuard(MutexGuard<size_t>& guard): mGuard{guard} {
        mGuard.lock([](size_t& connections) {
            connections++;
            std::cout << "Active Connections: " << connections << std::endl;
        });
    }

    ~ConnectionGuard() {
        mGuard.lock([](size_t& connections) {
            connections--;
            std::cout << "Active Connections: " << connections << std::endl;
        });
    }
};

auto handle_tx(HttpResponseGenerator& responseGenerator, IStream& stream) -> Task<Result<void>> {
    
    std::array<std::byte, 1024 * 4> buffer;
    while(true) {
        auto generatorResult = responseGenerator.generate(asio::mutable_buffer(buffer.data(), buffer.size()));
        if(generatorResult.has_error()) {
            co_return generatorResult.error();
        }

        auto& generatorOption = generatorResult.value();
        if(generatorOption.has_value() == false) {
            break;
        }

        size_t writeSize = generatorOption.value();

        auto writeResult = co_await stream.writeExact(buffer.data(), writeSize);
        if(writeResult.has_error()) {
            co_return writeResult.error();
        }
    }

    co_return Ok();
}

auto read_http_request(IStream& stream, std::byte* buf, const size_t size) -> Task<Result<size_t>> {
    size_t bytesWritten = 0;
    
    while(bytesWritten < 4 || memcmp(buf - 4, "\r\n\r\n", 4)) {
        const size_t bufLeft = size - bytesWritten;
        if(bufLeft == 0) {
            co_return Err("Not enough space for http request");
        }

        auto readResult = co_await stream.read(buf, bufLeft);
        if(readResult.has_error()) {
            co_return Err(readResult.error().what());
        }

        size_t readBytes = readResult.value();
        bytesWritten += readBytes;
        buf += readBytes;
    }

    co_return Ok(bytesWritten);
}

auto handle_client(const HttpServer& httpServer, IStream& stream) -> Task<void> {
    ConnectionGuard connectionGuard{activeConnections};
    std::array<std::byte, 1024 * 4> buffer;
    auto readResult = co_await read_http_request(stream, buffer.data(), buffer.size());
    if(readResult.has_error()) {
        Log("%s", readResult.error().what());
        co_return;
    }
    
    size_t bytesRead = readResult.value();

    auto generatorResult = httpServer.parse(std::string_view(reinterpret_cast<char*>(buffer.data()), bytesRead));
    if(generatorResult.has_error()) {
        Log("%s", generatorResult.error().what());
        co_return;
    }

    auto& generator = generatorResult.value();
    auto txResult = co_await handle_tx(generator, stream);
    if(txResult.has_error()) {
        Log("%s", txResult.error().what());
        co_return;
    }

    std::cout << "Requests Served: " << ++requestsServed << std::endl;
}

template <typename T>
auto handle_client_wrapper(const HttpServer& httpServer, T streamImpl) -> Task<void> {
    try {
        co_await handle_client(httpServer, streamImpl);
    } catch(const std::exception& err) {
        Log("%s", err.what());
    }
}

struct ServerArgs {
    std::filesystem::directory_entry directory;
    std::string host;
    std::string service;
};

ServerArgs parse_args(int argc, char *argv[]) {

    argparse::ArgumentParser parser(*argv, SERVER_VERSION);
	ServerArgs result;

	parser.add_argument("--path", "-p")
		.default_value(std::filesystem::current_path().string())
		.help("specify the http root path");

    parser.add_argument("--host", "-h")
		.default_value("127.0.0.1"s)
		.help("specify address to use");

    parser.add_argument("--service", "-s")
		.default_value("8080"s)
		.help("specify port");

	parser.parse_args(argc, argv);

	result.directory = std::filesystem::directory_entry(parser.get("--path"));
    result.host = parser.get("--host");
    result.service = parser.get("--service");

    return result;
}

} // namespace

int main(int argc, char *argv[]) {
    auto serverArgs = parse_args(argc, argv);
    auto httpServer = HttpServer(std::move(serverArgs.directory));

    asio::io_context ctx;
    auto executor = asio::any_io_executor(ctx.get_executor());
    
    auto tcpTask = std::bind(handle_client_wrapper<Tcp::TcpStream>, httpServer, std::placeholders::_1);
    auto tcpAcceptorResult = Tcp::TcpAcceptor::create(executor, tcpTask, serverArgs.host, serverArgs.service);
    auto& tcpAcceptor = tcpAcceptorResult.value();

    co_spawn(ctx, tcpAcceptor.accept(), detached);
    
    boost::thread_group tg;
    for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
        tg.create_thread(boost::bind(&asio::io_context::run, &ctx));
    }

    tg.join_all();
}
