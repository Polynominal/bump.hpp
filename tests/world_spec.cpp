#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "BumpWorldTest.hpp"




TEST_F(BumpWorldTest, ItemPlacement)
{
    auto itemCore = std::make_shared<TestItem>();
    auto item = world.Add(itemCore, Rectangle(1,1,1,1));
    ASSERT_EQ(item->UserData.GetRaw(), itemCore.get());
}

TEST_F(BumpWorldTest, ItemDuplicate)
{
    auto itemCore = std::make_shared<TestItem>();
    auto item = world.Add(itemCore, Rectangle(1,1,1,1));

    auto item2 = world.Add(itemCore, Rectangle(1,1,1,1));
    ASSERT_EQ(item2, nullptr);
}

TEST_F(BumpWorldTest, BasicCells)
{
    {
        auto itemCore = std::make_shared<TestItem>();
        world.Add(itemCore, Rectangle(0,0,10,10));
        ASSERT_EQ(world.CountCells(), 1);
        ASSERT_EQ(world.CountCellItems(), 1);
    }

    {
        auto itemCore = std::make_shared<TestItem>();
        world.Add(itemCore, Rectangle(100,100,10,10));
        ASSERT_EQ(world.CountCells(), 2);
        ASSERT_EQ(world.CountCellItems(), 2);
    }

    {
        auto itemCore = std::make_shared<TestItem>();
        world.Add(itemCore, Rectangle(0,0, 100,10));
        ASSERT_EQ(world.CountCells(), 3);
        ASSERT_EQ(world.CountCellItems(), 3);
    }

    {
        auto itemCore = std::make_shared<TestItem>();
        world.Add(itemCore, Rectangle(0,0,100,10));
        ASSERT_EQ(world.CountCells(), 3);
        ASSERT_EQ(world.CountCellItems(), 4);
    }

    {
        auto itemCore = std::make_shared<TestItem>();
        world.Add(itemCore, Rectangle(300,300, 64,64));
        ASSERT_EQ(world.CountCells(), 7);
        ASSERT_EQ(world.CountCellItems(), 5);
    }
}

TEST_F(BumpWorldTest, UpdateObject)
{

    auto itemCore = std::make_shared<TestItem>();
    auto item = world.Add(itemCore, Rectangle(0,0,10,10));
    auto rect = item->GetRectangle();
    rect.pos.x = 40;
    rect.pos.y = 40;
    rect.scale.x = 20;
    rect.scale.y = 20;

    world.Update(item, rect);
    CompareRectangle(item->GetRectangle(), rect);

}
TEST_F(BumpWorldTest, CollisionList)
{
    {
        AddItem("a", 0,0,10,10);
        AddItem("b", 20,20,10,10);
        ASSERT_EQ(world.CountItems(), 2);
        ASSERT_EQ(world.CountCellItems(), 2);

        Collisions collisions;
        world.Project(collisions, nullptr, Rectangle(4,6,10,10));
        ASSERT_EQ(collisions.size() ,1);
    }
}

TEST_F(BumpWorldTest, CollisionListTunneling)
{
    AddItem("a", 1,0,2,1);

    Collisions collisions;
    world.Project(collisions, nullptr, Rectangle(-5,0,4,1), math::vec2(5,0));
    ASSERT_EQ(collisions.size() ,1);
}

TEST_F(BumpWorldTest, CollisionTouchOnly)
{
    AddItem("b",0,0,32,100);
    Collisions collisions;
    world.Project(collisions, nullptr, Rectangle(32,50,20,20), math::vec2(30,50));
    ASSERT_EQ(collisions.size() ,1);
}
TEST_F(BumpWorldTest, SortedByTi)
{
    AddItem("b", 70,0, 10,10);
    AddItem("c", 50,0, 10,10);
    AddItem("d", 90,0, 10,10);

    Collisions collisions;
    world.Project(collisions, nullptr, Rectangle(110,0,10,10), math::vec2(10,0));

    std::vector<float> tis = {0.1f, 0.3f, 0.5f};
    int i=0;
    for (auto& col: collisions)
    {
        ASSERT_EQ(col->ti, tis[i]);
        i++;
    };
}
TEST_F(BumpWorldTest, CollisionFilter)
{
    AddItem("b", 70,0, 10,10);
    AddItem("c", 50,0, 10,10);

    auto d = AddItem("d", 90,0, 10,10);

    Collisions collisions;
    world.Project(collisions, nullptr, Rectangle(110,0,10,10), math::vec2(10,0), [d](Item* item, Item* other, auto response){
        return other != d;
    });
    ASSERT_EQ(collisions.size(), 2);
}

TEST_F(BumpWorldTest, Remove)
{
    auto a = AddItem("a", 0,0,10,10);

    {
        Collisions collisions;
        world.Project(collisions, nullptr, Rectangle(5,0,1,1));
        ASSERT_EQ(collisions.size(), 1);
    }


    world.Remove(a);

    {
        Collisions collisions;
        world.Project(collisions, nullptr, Rectangle(5,0,1,1));
        ASSERT_EQ(collisions.size(), 0);
    }

}
TEST_F(BumpWorldTest, RemoveClean)
{
    auto a = AddItem("a", 0,0,10,10);
    auto b = AddItem("b", 200,200, 10,10);
    ASSERT_EQ(world.CountCells(), 2);
    world.Remove(b);
    ASSERT_EQ(world.CountCells(), 1);

}

TEST_F(BumpWorldTest, ToCell)
{
    CompareVec2(world.ToCell(math::vec2(0,0)), math::vec2(1,1));
    CompareVec2(world.ToCell(math::vec2(63.9,63.9)), math::vec2(1,1));
    CompareVec2(world.ToCell(math::vec2(64,64)), math::vec2(2,2));
    CompareVec2(world.ToCell(math::vec2(-1,-1)), math::vec2(0,0));
}

TEST_F(BumpWorldTest, ToWorld)
{
    CompareVec2(world.ToWorld(math::vec2(1,1)), math::vec2(0,0));
    CompareVec2(world.ToWorld(math::vec2(2,2)), math::vec2(64,64));
    CompareVec2(world.ToWorld(math::vec2(-1,1)), math::vec2(-128,0));
}

TEST_F(BumpWorldTest, QueryRectEmpty)
{
    std::set<Item*> items;
    world.QueryRect(items, Rectangle(0,0,1,1));
    ASSERT_EQ(items.size(), 0);
}
TEST_F(BumpWorldTest, QueryRectItems)
{
    auto a = AddItem("a", 10,0, 10,10);
    auto b = AddItem("b", 70,0, 10,10);
    auto c = AddItem("c", 50,0, 10,10);
    auto d = AddItem("d", 90,0, 10,10);

    {
        std::set<Item*> items;
        world.QueryRect(items, Rectangle(55, 5, 20, 20));
        CompareSet(items, {b,c});
    }

    {
        std::set<Item*> items;
        world.QueryRect(items, Rectangle(0, 5, 100, 20));
        CompareSet(items, {a,b,c,d});
    }
}

TEST_F(BumpWorldTest, QueryPoint)
{
    {
        std::set<Item*> items;
        world.QueryPoint(items, math::vec2(0,0));
        ASSERT_EQ(items.size(), 0);
    }

    auto a = AddItem("a", 10,0, 10,10);
    auto b = AddItem("b", 15,0, 10,10);
    auto c = AddItem("c", 20,0, 10,10);

    QueryPointTest(4,5, {});
    QueryPointTest(14,5, {a});
    QueryPointTest(16,5, {a,b});
    QueryPointTest(21,5, {b,c});
    QueryPointTest(26,5, {c});
    QueryPointTest(31,5, {});

}
TEST_F(BumpWorldTest, QueryPointFilter)
{
    auto a = AddItem("a", 10,0, 10,10);
    auto b = AddItem("b", 15,0, 10,10);
    auto c = AddItem("c", 20,0, 10,10);

    auto filter = [b](auto other)
    {
        return other != b;
    };

    QueryPointTest(4,5, filter, {});
    QueryPointTest(14,5, filter, {a});
    QueryPointTest(16,5, filter, {a});
    QueryPointTest(21,5, filter, {c});
    QueryPointTest(26,5, filter, {c});
    QueryPointTest(31,5, filter, {});
}


TEST_F(BumpWorldTest, QuerySegment)
{
    AddItem("a", 10,0, 5,5);
    AddItem("c", 20,0, 5,5);

    QuerySegmentTest(0,5,  10,0,  {});
    QuerySegmentTest(15,5, 20,0,  {});
    QuerySegmentTest(26,5, 25,0,  {});
}

TEST_F(BumpWorldTest, QuerySegmentTouchOrder)
{
    auto a = AddItem("a",  5,0, 5,10);
    auto b = AddItem("b", 15,0, 5,10);
    auto c = AddItem("c", 25,0, 5,10);


    QuerySegmentTest(0,5, 11,5,  {a});
    QuerySegmentTest(0,5, 17,5,  {a,b});
    QuerySegmentTest(0,5, 30,5,  {a,b,c});
    QuerySegmentTest(17,5, 26,5, {b,c});
    QuerySegmentTest(22,5, 26,5, {c});

    QuerySegmentTest(11,5, 0,5,  {a});
    QuerySegmentTest(17,5, 0,5,  {b,a});
    QuerySegmentTest(30,5, 0,5,  {c,b,a});
    QuerySegmentTest(26,5, 17,5, {c,b});
    QuerySegmentTest(26,5, 22,5, {c});
}

TEST_F(BumpWorldTest, QuerySegmentFilter)
{
    auto a = AddItem("a",  5,0, 5,10);
    auto b = AddItem("b", 15,0, 5,10);
    auto c = AddItem("c", 25,0, 5,10);


    FilterSingle filter = [a,c](auto other){
        return other != a and other != c;
    };

    auto empty = std::set<Item*>();
    QuerySegmentTest(0,5, 11,5, filter,  empty);
    QuerySegmentTest(0,5, 17,5, filter,  {b});
    QuerySegmentTest(0,5, 30,5, filter,  {b});
    QuerySegmentTest(17,5, 26,5, filter, {b});
    QuerySegmentTest(22,5, 26,5, filter, empty);

    QuerySegmentTest(11,5, 0,5, filter,  empty);
    QuerySegmentTest(17,5, 0,5, filter,  {b});
    QuerySegmentTest(30,5, 0,5, filter,  {b});
    QuerySegmentTest(26,5, 17,5, filter, {b});
    QuerySegmentTest(26,5, 22,5, filter, empty);
}

TEST_F(BumpWorldTest, HasItem)
{
    auto itemCore = std::make_shared<TestItem>();
    ASSERT_FALSE(world.HasItem(itemCore));
    world.Add(itemCore, Rectangle(0,0,1,1));
    ASSERT_TRUE(world.HasItem(itemCore));
}

TEST_F(BumpWorldTest, GetItems)
{
    auto a = AddItem("a",  1,1, 1,1);
    auto b = AddItem("b",  2,2, 2,2);


    auto items = world.GetItems();

    ASSERT_EQ(items.size(), 2);

    std::set<Item*> itemSet;
    for (auto item: items)
    {
        itemSet.insert(item);
    }

    CompareSet(itemSet, {a,b});
}
TEST_F(BumpWorldTest, CountItems)
{
    auto a = AddItem("a",  1,1, 1,1);
    auto b = AddItem("b",  2,2, 2,2);

    ASSERT_EQ(world.CountItems(), 2);
}
TEST_F(BumpWorldTest, MoveNoCollision)
{
    auto a = AddItem("a",  1,1, 1,1);

    CollisionResolution resolution;
    world.Move(resolution, a, math::vec2(1,1));

    CompareVec2(resolution.pos, math::vec2(1,1));
    ASSERT_EQ(resolution.collisions.size(), 0);
}
