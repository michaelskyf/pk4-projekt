#pragma once

// TODO

namespace Server::Logger {
    #define Log(f, ...) printf(f"\n", __VA_ARGS__)
} // namespace Server::Logger