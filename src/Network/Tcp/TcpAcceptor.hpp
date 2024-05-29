#pragma once

#include "TcpStream.hpp"
#include <Common/Common.hpp>
#include <Memory/Mempool.hpp>
#include "TcpStream.hpp"
#include <boost/asio/any_io_executor.hpp>
#include <functional>

namespace Server::Network::Tcp {

class TcpAcceptor final {
    using CallbackType = std::function<Task<void>(TcpStream&&)>;
    asio::any_io_executor& mCtx;
    CallbackType mCallback;
    asio::ip::tcp::acceptor mAcceptor;

    TcpAcceptor(asio::any_io_executor& ctx, CallbackType callback, asio::ip::tcp::acceptor&& acceptor)
    : mCtx{ctx}, mCallback{callback}, mAcceptor{std::move(acceptor)} {

    }

public:

    static auto create(asio::any_io_executor& ctx, CallbackType callback, std::string_view host, std::string_view service) -> Result<TcpAcceptor>;

    auto accept() -> Task<void>;
};

} // namespace Server::Network::Tcp