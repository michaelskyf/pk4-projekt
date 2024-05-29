#include <gtest/gtest.h>

#include "../MutexGuard.hpp"

TEST(MutexGuard, idk)
{
    MutexGuard<int> guard{8};
    
    auto result = guard.lock([](auto &x){
        return x + 3;
    });

    ASSERT_EQ(result, 11);
}

TEST(MutexGuard, idk_try_lock)
{
    MutexGuard<int> guard{8};
    
    auto result = guard.tryLock([](auto &x){
        return x + 3;
    });

    ASSERT_EQ(result, 11);
}