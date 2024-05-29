#pragma once

#include "../IStream.hpp"

namespace Server::Network::Tcp {

class TcpStream final : public IStream {

    asio::ip::tcp::socket mSocket;

public:

    TcpStream(boost::asio::ip::tcp::socket&&);

    virtual auto readExact(std::byte* buf, size_t size) -> Task<Result<void>> override;
    virtual auto read(std::byte* buf, size_t size) -> Task<Result<size_t>> override;
    virtual auto writeExact(const std::byte* data, size_t size) -> Task<Result<void>> override;
};

} // namespace Server::Network::Tcp