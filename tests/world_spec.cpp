#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "BumpWorldTest.hpp"
#include "../CollisionResponses.hpp"




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

// ============================================================
// Bug 6 fix: GetCellsTouchedBySegment used `y` as column key
// instead of `x`, so QuerySegment never found items in cells
// that were not in the same row+column position by coincidence.
// These tests use items spread across clearly distinct cells
// and verify the segment actually finds them.
// ============================================================

TEST_F(BumpWorldTest, QuerySegmentAcrossCells)
{
    // Items placed in different cells (cell size = 64)
    // a is in cell (1,1), b is in cell (3,1), c is in cell (5,1)
    // A horizontal segment at y=5 crossing all of them should find all three.
    auto a = AddItem("a",  10, 0, 10, 10);  // cell (1,1)
    auto b = AddItem("b", 150, 0, 10, 10);  // cell (3,1)
    auto c = AddItem("c", 300, 0, 10, 10);  // cell (5,1)

    // Segment from left of a to right of c
    QuerySegmentTest(0, 5, 320, 5, {a, b, c});
}

TEST_F(BumpWorldTest, QuerySegmentVertical)
{
    // Items in different row cells: cell (1,1), (1,3), (1,5)
    auto a = AddItem("a", 0,  10, 10, 10);  // cell (1,1)
    auto b = AddItem("b", 0, 150, 10, 10);  // cell (1,3)
    auto c = AddItem("c", 0, 300, 10, 10);  // cell (1,5)

    // Vertical segment crossing all
    QuerySegmentTest(5, 0, 5, 320, {a, b, c});
}

TEST_F(BumpWorldTest, QuerySegmentSingleDistantCell)
{
    // Item in a far cell — with the bug, looking up column x using key y
    // would always miss items not on the diagonal.
    auto a = AddItem("a", 200, 10, 10, 10);  // cell (4,1) — x != y

    QuerySegmentTest(190, 15, 220, 15, {a});
}

// ============================================================
// Bug 4 fix: World::Update re-add loop checked new rect bounds
// instead of old rect bounds, so items moving into overlapping
// cell regions were never added to the new cells.
// After Update, the item must still be detectable via Project.
// ============================================================

TEST_F(BumpWorldTest, UpdateItemStillCollidable)
{
    // Bug 2 only triggers when moving within the SAME ROW to a different column.
    // The buggy re-add condition (cx < new_cl or cx > new_cr) is always false
    // for cx values inside the new rect, so when cyOut is also false (same row),
    // nothing gets added. Moving to a different row bypasses the bug (cyOut=true).
    // old: (0,0,10,10) -> col=1, row=1. new: (200,0,10,10) -> col=4, row=1.
    // cy=1: cyOut=false. cx=4: buggy cx<4 or cx>4 = false -> NOT added. Bug triggered.
    auto a = AddItem("a", 0, 0, 10, 10);   // col=1, row=1

    Rectangle newRect(200, 0, 10, 10);     // col=4, row=1 — same row!
    world.Update(a, newRect);

    // Probe at new position — item should be found only if col=4 was correctly added
    Collisions collisions;
    world.Project(collisions, nullptr, Rectangle(205, 5, 1, 1));
    ASSERT_EQ(collisions.size(), 1);
}

TEST_F(BumpWorldTest, UpdateItemNotAtOldPosition)
{
    // Same-row move (y=0 unchanged): old col=1, new col=4.
    // With the bug, item is removed from col=1 correctly but never added to col=4.
    // So old position returns 0 (correct either way) but new position also returns 0 (wrong).
    auto a = AddItem("a", 0, 0, 10, 10);   // col=1, row=1

    Rectangle newRect(200, 0, 10, 10);     // col=4, row=1 — same row!
    world.Update(a, newRect);

    // Old position: item should be gone
    Collisions oldPos;
    world.Project(oldPos, nullptr, Rectangle(5, 0, 1, 1));
    ASSERT_EQ(oldPos.size(), 0);

    // New position: item should be found — fails with bug because col=4 was never added
    Collisions newPos;
    world.Project(newPos, nullptr, Rectangle(205, 0, 1, 1));
    ASSERT_EQ(newPos.size(), 1);
}

TEST_F(BumpWorldTest, UpdateCellCountCorrect)
{
    // Same-row move: old col=1 row=1, new col=4 row=1.
    // With the bug, col=4 is never added (cyOut=false, cx condition always false).
    // So after Update, CountCells drops to 0 instead of staying at 1.
    auto a = AddItem("a", 0, 0, 10, 10);   // col=1, row=1
    ASSERT_EQ(world.CountCells(), 1);

    Rectangle newRect(200, 0, 10, 10);     // col=4, row=1 — same row!
    world.Update(a, newRect);

    // Old cell removed, new cell added — should still be exactly 1
    ASSERT_EQ(world.CountCells(), 1);
}

// ============================================================
// Bug 2 fix: Grid::Traverse called f(center) instead of
// f(cBottom) for the final cell, so the destination cell
// was sometimes missed when the traversal loop exited early.
// QuerySegment hitting exactly the last cell exercises this.
// ============================================================

TEST_F(BumpWorldTest, TraverseFinalCellReached)
{
    // Item placed so that the segment's endpoint lands exactly
    // in its cell and the loop exits before visiting it.
    // The item sits in a cell that is the last cell of the segment.
    auto a = AddItem("a", 250, 0, 10, 10);  // cell (4,1)

    // Segment that ends just inside a's cell
    QuerySegmentTest(0, 5, 256, 5, {a});
}

TEST_F(BumpWorldTest, TraverseFinalCellDiagonal)
{
    // Segment ends exactly 1 step away from the last visited cell,
    // so the while-loop exits with center != cBottom and the final-cell
    // branch must call f(cBottom) not f(center). With the bug, f(center)
    // was called — visiting the wrong cell and missing the item.
    // Item is in cell (4,1); segment travels horizontally from cell (1,1)
    // to cell (4,1), exiting the loop one cell early.
    auto a = AddItem("a", 250, 0, 10, 10);  // cell (4,1) x=250..260

    // Segment ends at x=255, well inside cell (4,1) — exactly the final-cell case
    QuerySegmentTest(0, 5, 255, 5, {a});
}
