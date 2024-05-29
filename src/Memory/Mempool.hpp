#pragma once

#include <array>
#include <boost/optional/optional.hpp>
#include <list>
#include <memory>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

#include "Common/Common.hpp"
#include "Common/MutexGuard.hpp"
#include "MemoryGuard.hpp"

namespace Server::Memory {

template <typename T>
class Mempool {

    using DataType = std::aligned_storage<sizeof(T), alignof(T)>::type;

    MutexGuard<std::list<DataType*>> mRing; // TODO: Real ring
    std::vector<DataType> mData;
    
    Mempool(std::list<DataType*>&& ring, std::vector<DataType>&& data)
        : mRing{std::move(ring)}, mData{std::move(data)} {

    }

    Mempool(Mempool&&) = delete;

public:
    Mempool() = delete; // Mempool::create is the constructor
    ~Mempool() = default;
    
    Mempool(const Mempool&) = delete;

    static auto create(size_t dataSize) -> Mempool {
        auto data = std::vector<DataType>(dataSize);

        auto ring = std::list<DataType*>();
        for(size_t i = 0; i < dataSize; i++) {
            ring.push_back(&data[i]);
        }

        return Mempool(std::move(ring), std::move(data));
    }

    auto sizeLeft() -> size_t {
        return mRing.lock([&](const std::list<DataType*>& ring) -> size_t {
            return ring.size();
        });
    }

    auto totalSize() const -> size_t {
        return mData.size();
    }

    template <size_t ArraySize>
    requires(ArraySize > 0)
    Option<MemoryGuard<T, ArraySize>> get() {
        return mRing.lock([&](std::list<DataType*>& ring) -> Option<MemoryGuard<T, ArraySize>> {
            if(ring.size() == 0) return None;

            std::array<DataType*, ArraySize> ptrArray;

            for(size_t i = 0; i < ArraySize; i++) {
                DataType* ptr = ring.front();
                ring.pop_front();

                ptrArray[i] = ptr;
            }

            auto returnMemoryFunc = [mempool=this](DataType* data) -> void{ mempool->put(data); };
            return MemoryGuard<T, ArraySize>(returnMemoryFunc, ptrArray);
        });
    }

private:

    void put(DataType* data) {
        mRing.lock([&](std::list<DataType*>& ring){
            ring.push_back(data);
        });
    }
};

} // namespace Server::Memory