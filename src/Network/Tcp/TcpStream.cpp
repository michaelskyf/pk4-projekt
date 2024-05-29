#include "TcpStream.hpp"
#include <boost/outcome/success_failure.hpp>

using namespace Server::Network::Tcp;

TcpStream::TcpStream(boost::asio::ip::tcp::socket&& socket)
    : mSocket{std::move(socket)} {

}

auto TcpStream::readExact(std::byte* buf, size_t size) -> Task<Result<void>> {
    constexpr auto tok = as_tuple(boost::asio::use_awaitable);
    
    while(size > 0) {
        auto readBuffer = asio::buffer(buf, size);
        auto[errorCode, readBytes] = co_await mSocket.async_receive(readBuffer, tok);
        if(errorCode) {
            co_return Err(errorCode.what());
        }
        size -= readBytes;
        buf += readBytes;
    }
    
    co_return Ok();
}

auto TcpStream::read(std::byte* buf, size_t size) -> Task<Result<size_t>> {
    constexpr auto tok = as_tuple(boost::asio::use_awaitable);
    
    auto readBuffer = asio::buffer(buf, size);
    auto[errorCode, readBytes] = co_await mSocket.async_receive(readBuffer, tok);
    if(errorCode) {
        co_return Err(errorCode.what());
    }
    
    co_return Ok(readBytes);
}

auto TcpStream::writeExact(const std::byte* data, size_t size) -> Task<Result<void>> {
    auto tok = as_tuple(boost::asio::use_awaitable);
    
    while(size != 0)
    {
        auto[ec, tmp] = co_await mSocket.async_send(boost::asio::buffer(data, size), tok);
        if(ec)
        {
            co_return Err(ec.what());
        }

        size -= tmp;
        data += tmp;
    }

    co_return Ok();
}