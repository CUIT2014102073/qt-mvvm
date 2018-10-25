#include <gtest/gtest.h>
#include "itempool.h"
#include "sessionitem.h"
#include <memory>

using namespace ModelView;

//! Tests of ItemPool and its abilities to register/deregister SessionItem.

class TestItemPool : public ::testing::Test
{
public:
    ~TestItemPool();
};

TestItemPool::~TestItemPool() = default;

TEST_F(TestItemPool, initialState)
{
    std::unique_ptr<ItemPool> pool(new ItemPool);
    EXPECT_EQ(pool->size(), 0u);
}

//! Explicit item registrations.

TEST_F(TestItemPool, registerItem)
{
    std::unique_ptr<ItemPool> pool(new ItemPool);
    std::unique_ptr<SessionItem> item(new SessionItem);

    // registering item
    auto key = pool->register_item(item.get());
    EXPECT_EQ(pool->size(), 1u);
    EXPECT_FALSE(key.empty());

    // checking registered key and item
    EXPECT_EQ(key, pool->key_for_item(item.get()));
    EXPECT_EQ(item.get(), pool->item_for_key(key));

    // checking unexisting registration
    std::unique_ptr<SessionItem> item2(new SessionItem);
    EXPECT_EQ(ItemPool::key_type(), pool->key_for_item(item2.get()));
    EXPECT_EQ(nullptr, pool->item_for_key("ABC"));

    // registering second item
    auto key2 = pool->register_item(item2.get());
    EXPECT_EQ(pool->size(), 2u);
    EXPECT_EQ(key2, pool->key_for_item(item2.get()));
    EXPECT_FALSE(key == key2);

    // attempt to register item twice
    EXPECT_THROW(pool->register_item(item2.get()), std::runtime_error);
}

//! Explicit item de-registrations.

TEST_F(TestItemPool, deregisterItem)
{
    std::unique_ptr<ItemPool> pool(new ItemPool);
    std::unique_ptr<SessionItem> item1(new SessionItem);
    std::unique_ptr<SessionItem> item2(new SessionItem);

    auto key1 = pool->register_item(item1.get());
    auto key2 = pool->register_item(item2.get());

    EXPECT_EQ(pool->size(), 2u);
    EXPECT_EQ(item1.get(), pool->item_for_key(key1));
    EXPECT_EQ(item2.get(), pool->item_for_key(key2));

    // deregistering item
    pool->deregister_item(item1.get());
    EXPECT_EQ(pool->size(), 1u);
    EXPECT_EQ(nullptr, pool->item_for_key(key1));
    EXPECT_EQ(item2.get(), pool->item_for_key(key2));

    // attempt to deregister twice
    EXPECT_THROW(pool->deregister_item(item1.get()), std::runtime_error);

    // deregistering last remaining item
    pool->deregister_item(item2.get());
    EXPECT_EQ(pool->size(), 0u);
}

//! Item registrations via method call.

TEST_F(TestItemPool, sessionItemRegistration)
{
    std::shared_ptr<ItemPool> pool(new ItemPool);
    EXPECT_EQ(pool.use_count(), 1l);

    // explicit item registration
    auto item = new SessionItem;
    item->register_item(pool);
    EXPECT_EQ(pool.use_count(), 1l); // weak ptr inside of SessionItem
    EXPECT_EQ(pool->size(), 1u);
    EXPECT_FALSE(pool->key_for_item(item).empty());

    // deleting item
    delete item;
    EXPECT_EQ(pool->size(), 0u);
}

//! Checks what happened if pool deleted before item

TEST_F(TestItemPool, poolTimeOfLife)
{
    auto item = new SessionItem;

    {
        std::shared_ptr<ItemPool> pool(new ItemPool);
        item->register_item(pool);
    }

    EXPECT_NO_THROW(delete item);
}

//!Providing custom key.

TEST_F(TestItemPool, customKey)
{
    std::shared_ptr<ItemPool> pool(new ItemPool);
    EXPECT_EQ(pool.use_count(), 1l);

    // explicit item registration
    const identifier_type id("abc-cde-fgh");
    auto item = new SessionItem;
    pool->register_item(item, id);

    // attempt to reuse key again
    std::unique_ptr<SessionItem> item2(new SessionItem);
    EXPECT_THROW(pool->register_item(item2.get(), id), std::runtime_error);
}
