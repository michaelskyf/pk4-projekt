#pragma once

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/none.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/outcome/result.hpp>
#include <boost/outcome/success_failure.hpp>
#include <stdexcept>

#include <Logger/Logger.hpp>

namespace Log = Server::Logger;
namespace asio = boost::asio;

using std::operator""sv;

template<typename T>
using Task = boost::asio::awaitable<T>;

template<typename T>
using Option = std::optional<T>;

const auto None = std::nullopt;

template <typename T>
auto Some = boost::make_optional<T>;

template<typename T>
using Result = boost::outcome_v2::result<T, std::runtime_error>;

#define Ok boost::outcome_v2::success
#define Err boost::outcome_v2::failure