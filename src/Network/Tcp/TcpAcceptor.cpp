#include "TcpAcceptor.hpp"
#include <boost/asio/use_awaitable.hpp>
#include <iostream>

using namespace Server::Network::Tcp;

auto TcpAcceptor::create(asio::any_io_executor& ctx, CallbackType callback, std::string_view host, std::string_view service) -> Result<TcpAcceptor> {
        try {
            auto resolver = asio::ip::tcp::resolver(ctx);
            auto query = asio::ip::tcp::resolver::query(host.data(), service.data());
            auto queryResult = resolver.resolve(query);
            auto endpoint = queryResult->endpoint();
            auto acceptor = asio::ip::tcp::acceptor(ctx, endpoint);

            std::cout << "Created acceptor at " << host << ':' << service << std::endl;
            return TcpAcceptor(ctx, callback, std::move(acceptor));
        } catch(const std::runtime_error& err) {
            return Err(err.what());
        }
    }

auto TcpAcceptor::accept() -> Task<void> {

    while(true) { // TODO: Shutdown?
        auto clientSocket = co_await mAcceptor.async_accept(asio::use_awaitable);
        clientSocket.set_option(asio::socket_base::keep_alive(true));

        asio::co_spawn(mCtx, mCallback(TcpStream(std::move(clientSocket))), asio::detached);
    }

    co_return;
}