#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Rectangle.hpp"
#include "../Collision.hpp"

using namespace plugin::physics::bump;

TEST(RECTANGLE, StaticIntersectTest)
{
    {
        std::shared_ptr<Collision> col = std::make_shared<Collision>();
        ASSERT_FALSE(Rectangle(0,0,1,1).DetectCollision(Rectangle(5,5,1,1), col.get(), math::vec2(0,0)));
    }

    {
        std::shared_ptr<Collision> col = std::make_shared<Collision>();
        ASSERT_TRUE(Rectangle(5,5,1,1).DetectCollision(Rectangle(0,0,7,6), col.get(), math::vec2(5,5)));
        ASSERT_TRUE(Rectangle(0,0,7,6).DetectCollision(Rectangle(5,5,1,1), col.get(), math::vec2(0,0)));


        ASSERT_EQ(col->ti, -2.0f);

        ASSERT_EQ(col->move.x, 0.0f);
        ASSERT_EQ(col->move.y, 0.0f);

        ASSERT_EQ(col->itemRect.pos.x, 0.0f);
        ASSERT_EQ(col->itemRect.pos.y, 0.0f);
        ASSERT_EQ(col->itemRect.scale.x, 7.0f);
        ASSERT_EQ(col->itemRect.scale.y, 6.0f);

        ASSERT_EQ(col->otherRect.pos.x, 5.0f);
        ASSERT_EQ(col->otherRect.pos.y, 5.0f);
        ASSERT_EQ(col->otherRect.scale.x, 1.0f);
        ASSERT_EQ(col->otherRect.scale.y, 1.0f);

        ASSERT_EQ(col->normal.x, 0.0f);
        ASSERT_EQ(col->normal.y, -1.0f);
    }
}

TEST(RECTANGLE, StaticIntersectTestExtra)
{
    {
        std::shared_ptr<Collision> col = std::make_shared<Collision>();;
        ASSERT_FALSE(Rectangle(4,6,10,10).DetectCollision(Rectangle(20,20,10,10), col.get(), math::vec2(0,0)));
        ASSERT_FALSE(Rectangle(20,20,10,10).DetectCollision(Rectangle(4,6,10,10),  col.get()));
    }
}

TEST(RECTANGLE, MovingIntersectTest)
{
    {
        std::shared_ptr<Collision> col = std::make_shared<Collision>();;
        ASSERT_FALSE(Rectangle(0,0,1,1).DetectCollision(Rectangle(5,5,1,1),  col.get(), math::vec2(0,1)));
    }

}


TEST(RECTANGLE, MovingIntersectTestLeft)
{
    std::shared_ptr<Collision> col = std::make_shared<Collision>();;
    ASSERT_TRUE(Rectangle(1,1,1,1).DetectCollision(Rectangle(5,0,1,1),  col.get(), math::vec2(6,0)));
    ASSERT_EQ(col->ti, 0.6f);
    ASSERT_EQ(col->normal.x, -1.0f);
    ASSERT_EQ(col->normal.y, 0.0f);
}
TEST(RECTANGLE, MovingIntersectTestRight)
{
    std::shared_ptr<Collision> col = std::make_shared<Collision>();;
    ASSERT_TRUE(Rectangle(6,0,1,1).DetectCollision(Rectangle(1,0,1,1),  col.get(), math::vec2(1,1)));
    ASSERT_FALSE(col->overlaps);
    ASSERT_EQ(col->ti, 0.8f);
    ASSERT_EQ(col->normal.x, 1.0f);
    ASSERT_EQ(col->normal.y, 0.0f);
}

TEST(RECTANGLE, MovingIntersectTestTop)
{
    std::shared_ptr<Collision> col = std::make_shared<Collision>();;
    ASSERT_TRUE(Rectangle(0,0,1,1).DetectCollision(Rectangle(0,4,1,1),  col.get(), math::vec2(0,5)));
    ASSERT_FALSE(col->overlaps);
    ASSERT_EQ(col->ti, 0.6f);
    ASSERT_EQ(col->normal.x, 0.0f);
    ASSERT_EQ(col->normal.y, -1.0f);
}

TEST(RECTANGLE, MovingIntersectTestBottom)
{
    std::shared_ptr<Collision> col = std::make_shared<Collision>();;
    ASSERT_TRUE(Rectangle(0,4,1,1).DetectCollision(Rectangle(0,0,1,1),  col.get(), math::vec2(0,-1)));
    ASSERT_FALSE(col->overlaps);
    ASSERT_EQ(col->ti, 0.6f);
    ASSERT_EQ(col->normal.x, 0.0f);
    ASSERT_EQ(col->normal.y, 1.0f);
}


TEST(RECTANGLE, CornerCase)
{
    {
        std::shared_ptr<Collision> col = std::make_shared<Collision>();;
        ASSERT_FALSE(Rectangle(0,16,16,16).DetectCollision(Rectangle(16,0,16,16),  col.get(), math::vec2(-1,15)));
    }
}

// ============================================================
// Bug 1 fix: Rectangle::compare compared pos==pos and pos==scale
// instead of pos==rhs.pos and scale==rhs.scale.
// ============================================================

TEST(RECTANGLE, CompareIdentical)
{
    // Two identical rectangles must compare equal
    ASSERT_TRUE(Rectangle(0, 0, 1, 1).compare(Rectangle(0, 0, 1, 1)));
    ASSERT_TRUE(Rectangle(5, 3, 10, 20).compare(Rectangle(5, 3, 10, 20)));
}

TEST(RECTANGLE, OperatorEqAndNeq)
{
    Rectangle r1(1, 2, 3, 4);
    Rectangle r2(1, 2, 3, 4);
    Rectangle r3(1, 2, 3, 9);

    ASSERT_TRUE(r1 == r2);
    ASSERT_FALSE(r1 != r2);
    ASSERT_FALSE(r1 == r3);
    ASSERT_TRUE(r1 != r3);
}
