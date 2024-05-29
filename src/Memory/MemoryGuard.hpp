#pragma once

#include <boost/outcome/success_failure.hpp>
#include <cstddef>
#include <array>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <Common/Common.hpp>

namespace Server::Memory {

template <typename T, size_t Size>
class MemoryGuard {

    using Data = std::aligned_storage<sizeof(T), alignof(T)>::type;
    using PutType = std::function<void(Data*)>;

    PutType mPut;
    std::array<Data*, Size> mData;
    bool initialized = true;

public:
    MemoryGuard(PutType&& put, const std::array<Data*, Size>& data) 
    : mPut{put}, mData{data} {
        
    }

    ~MemoryGuard() {
        if(initialized == false) return;
        
        initialized = false;
    }

    MemoryGuard(MemoryGuard&& other)
    : mPut{other.mPut}, mData{other.mData} {
        other.initialized = false;
    }

    MemoryGuard(const MemoryGuard&) = delete;

    template <typename... Args>
    auto initialize(Args&&... args) -> Result<std::unique_ptr<T, std::function<void(T*)>>> {
        try {
            new(mData[0]) T(std::forward<Args>(args)...);
        } catch(const std::runtime_error& err) {
            return Err(err.what());
        }
  
        auto freeMemoryFunc = mPut;
        auto deleter = [freeMemoryFunc](T* obj) {
            std::destroy_at(obj);
            freeMemoryFunc(reinterpret_cast<Data*>(obj));
        };

        T* ptr = std::launder(reinterpret_cast<T*>(mData[0]));
        return std::unique_ptr<T, std::function<void(T*)>>(ptr, deleter);
    } 
};

} // namespace Server::Memory