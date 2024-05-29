#pragma once

#include <boost/asio/multiple_exceptions.hpp>
#include <mutex>

#include <Common/Common.hpp>

// TODO: Concepts
template <typename T>
class MutexGuard {
    std::mutex mutex;
    T mObj; // Compiler error? (Had to move it here to make it work)

public:
    MutexGuard() = default;
    ~MutexGuard() = default;
    MutexGuard(const MutexGuard&) = delete;
    MutexGuard(MutexGuard&&) = delete;
    MutexGuard(T&& obj): mObj{obj} {}

    template <typename F>
    auto lock(F&& func) -> decltype(func(this->mObj)) {
        auto lock = std::lock_guard(mutex);

        return func(mObj);
    }

    template <typename F>
    auto tryLock(F&& func) -> Option<decltype(func(this->mObj))> {
        if(mutex.try_lock() == false) return None;

        auto result = func(mObj);

        mutex.unlock();
        return result;
    }
};