#pragma once

#include <Common/Common.hpp>

namespace Server::Network {

class IStream {
public:
    virtual ~IStream() = default;

    /**
     * @brief Read exact amount of bytes from the stream
     * @param[in] buf   Pointer to a buffer
     * @param[in] size  Size of the buffer
     * @returns
     *          - void on success
     *          - error on failure
     */
    virtual auto readExact(std::byte* buf, size_t size) -> Task<Result<void>> = 0;
    virtual auto read(std::byte* buf, size_t size) -> Task<Result<size_t>> = 0;
    
    /**
     * @brief Write exact amount of bytes to the stream
     * @param[in] data   Pointer to the data
     * @param[in] size  Size of the data
     * @returns
     *          - void on success
     *          - error on failure
     */
    virtual auto writeExact(const std::byte* data, size_t size) -> Task<Result<void>> = 0;
};

} // namespace Server::Network
