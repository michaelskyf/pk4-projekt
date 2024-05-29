#include <gtest/gtest.h>
#include <memory>

#include "../Mempool.hpp"

using namespace Server::Memory;

TEST(Mempool, idk)
{
    auto mempool = Mempool<int>::create(1);

    auto good = mempool.get<1>();
    auto bad = mempool.get<1>();

    ASSERT_TRUE(good.has_value());
    ASSERT_FALSE(bad.has_value());
    EXPECT_EQ(mempool.sizeLeft(), 0);
}

TEST(Mempool, idk2)
{
    auto mempool = Mempool<int>::create(32);

    auto good = mempool.get<32>();
    auto bad = mempool.get<1>();

    ASSERT_TRUE(good.has_value());
    ASSERT_FALSE(bad.has_value());
    EXPECT_EQ(mempool.sizeLeft(), 0);
}

TEST(Mempool, object) {
    class X {
        bool& mDestructed;
        int mX;
    public:
        X() = delete;
        X(bool& destructed, int x): mDestructed{destructed}, mX{x} {

        }

        ~X() {
            mDestructed = true;
        }

        int get() const { return mX; }
    };

    auto mempool = Mempool<X>::create(32);
    auto memguard = mempool.get<1>();
    EXPECT_EQ(mempool.sizeLeft(), 31);
    bool destructed = false;
    {
        const int expectedNumber = 9;
        auto result = memguard->initialize(destructed, expectedNumber);
        auto& ptr = result.value();
        auto& x = *ptr;

        EXPECT_EQ(x.get(), expectedNumber);
    }

    EXPECT_TRUE(destructed);
    EXPECT_EQ(mempool.sizeLeft(), 32);
}

TEST(Mempool, RepeatedDeletionAndInsertion) {
    
    struct TestStruct {
        std::string id;
        ~TestStruct() {
            id = "0";
        }
    };

    auto mempool = Mempool<TestStruct>::create(1);
    for(size_t i = 0; i < 10000; i++) {
        auto memguard = mempool.get<1>();
        auto ptr = memguard->initialize();
    }

}