#include "BumpWorldTest.hpp"
#include "../CollisionResponses.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST_F(BumpWorldTest, BasicTouch)
{
    auto a  = AddItem("a", 0,0,1,1);
    auto b = AddItem("b", 0,2,1,1);
    auto c = AddItem("c", 0,3,1,1);


    CollisionResolution resolution;
    world.Move(resolution, a, math::vec2(0,5),[](auto item, auto other, Collision* col){

        col->Respond<plugin::physics::bump::response::Touch>();

        return true;
    });
    CompareVec2(resolution.pos, math::vec2(0,1));
    CompareRectangle(Rectangle(0,1,1,1), a->GetRectangle());
    ASSERT_EQ(resolution.collisions.size() ,1);

    auto col = Collision();
    col.other = b;
    col.Respond<response::Touch>();

    ASSERT_TRUE(resolution.collisions[0].Is<response::Touch>());
    ASSERT_EQ(resolution.collisions[0].other, b);
}


TEST_F(BumpWorldTest, BasicCross)
{
    auto a = AddItem("a", 0,0,1,1);
    auto b = AddItem("b", 0,2,1,1);
    auto c = AddItem("c", 0,3,1,1);

    CollisionResolution resolution;
    world.Move(resolution, a, math::vec2(0,5),[](auto item, auto other, Collision* col){
        col->Respond<plugin::physics::bump::response::Cross>();
        return true;
    });
    CompareVec2(resolution.pos, math::vec2(0,5));
    CompareRectangle(Rectangle(0,5,1,1), a->GetRectangle());
    ASSERT_EQ(resolution.collisions.size(), 2);

    ASSERT_TRUE(resolution.collisions[0].Is<response::Cross>());
    ASSERT_EQ(resolution.collisions[0].other, b);

    ASSERT_TRUE(resolution.collisions[1].Is<response::Cross>());
    ASSERT_EQ(resolution.collisions[1].other, c);
}

TEST_F(BumpWorldTest, BasicSliding)
{
    auto a = AddItem("a", 0,0,1,1);
    auto b = AddItem("b", 0,2,1,2);
    auto c = AddItem("c", 2,1,1,1);

    CollisionResolution resolution;
    world.Move(resolution, a, math::vec2(5,5),[](auto item, auto other, Collision* col){
        col->Respond<plugin::physics::bump::response::Slide>();
        return true;
    });

    CompareVec2(resolution.pos, math::vec2(1,5));
    CompareRectangle(Rectangle(1,5,1,1), a->GetRectangle());
    ASSERT_EQ(resolution.collisions.size(), 1);

    ASSERT_TRUE(resolution.collisions[0].Is<response::Slide>());
    ASSERT_EQ(resolution.collisions[0].other, c);
}

TEST_F(BumpWorldTest, BasicBounce)
{
    auto a = AddItem("a", 0,0,1,1);
    auto b = AddItem("b", 0,2,1,2);

    CollisionResolution resolution;
    world.Move(resolution, a, math::vec2(0,5),[](auto item, auto other, Collision* col){
        col->Respond<plugin::physics::bump::response::Bounce>();
        return true;
    });

    CompareVec2(resolution.pos, math::vec2(0,-3));
    ASSERT_EQ(resolution.collisions.size(), 1);
    CompareRectangle(Rectangle(0,-3,1,1), a->GetRectangle());
    ASSERT_TRUE(resolution.collisions[0].Is<response::Bounce>());
    ASSERT_EQ(resolution.collisions[0].other, b);
}









World sampleWorld = World();

std::vector<math::vec2> touch(float x, float y, float w, float h,  float ox, float oy, float ow, float oh, float goalX,float goalY)
{
    auto touch = response::Touch();
    touch.SetWorld(&sampleWorld);
    Collision col;
    Rectangle(x,y,w,h).DetectCollision(Rectangle(ox,oy,ow,oh), col, math::vec2(goalX,goalY));
    return {col.touch, col.normal};
}
auto touch(float x, float y, float w, float h,  float ox, float oy, float ow, float oh)
{
    return touch(x,y,w,h,ox,oy,ow,oh, x,y);
}

std::vector<math::vec2> slide(float x, float y, float w, float h,  float ox, float oy, float ow, float oh, float goalX,float goalY)
{
    Collision col;
    Rectangle(x,y,w,h).DetectCollision(Rectangle(ox,oy,ow,oh), col, math::vec2(goalX,goalY));

    CollisionResolution res;

    auto resp = response::Slide();
    resp.SetWorld(&sampleWorld);
    resp.Resolve(res,col, Rectangle(x,y,w,h), math::vec2(goalX,goalY),nullptr);
    return {col.touch, col.normal, resp.slide};
}
auto slide(float x, float y, float w, float h,  float ox, float oy, float ow, float oh)
{
    return slide(x,y,w,h,ox,oy,ow,oh, x,y);
}

std::vector<math::vec2> bounce(float x, float y, float w, float h,  float ox, float oy, float ow, float oh, float goalX,float goalY)
{
    Collision col;
    Rectangle(x,y,w,h).DetectCollision(Rectangle(ox,oy,ow,oh), col, math::vec2(goalX,goalY));

    CollisionResolution res;

    auto resp = response::Bounce();
    resp.SetWorld(&sampleWorld);
    resp.Resolve(res,col, Rectangle(x,y,w,h), math::vec2(goalX,goalY),nullptr);
    return {col.touch, col.normal, resp.bounce};
}
auto bounce(float x, float y, float w, float h,  float ox, float oy, float ow, float oh)
{
    return bounce(x,y,w,h,ox,oy,ow,oh, x,y);
}


std::vector<math::vec2> vec2Array(std::vector<float> floatArray)
{
    std::vector<math::vec2> array;
    for (int i=0; i < floatArray.size(); i=i+2)
    {
        array.push_back(math::vec2(floatArray[i],floatArray[i+1]));
    }
    return array;
}


TEST(TOUCH, still)
{
//     --                                          -2-1 0 1 2 3 4 5 6 7 8 9 10
// --      -2 -1 0 1 2 3 4 5 6 7 8 9           -2 · ┌–––┐ · ┌–––┐ · ┌–––┐ ·
// --      -1  ┌–––┐ · ┌–––┐ · ┌–––┐           -1 · │0-1│ · │0-1│ · │0-1│ ·
// --       0  │ ┌–––––––––––––––┐ │ 1  2  3    0 · └–┌–––––––––––––––┐–┘ ·
// --       1  └–│–┘ · └–––┘ · └–│–┘            1 · · │ · · · · · · · │ · ·
// --       2  · │ · · · · · · · │ ·            2 · · │ · · · · · · · │ · ·
// --       3  ┌–│–┐ · ┌–––┐ · ┌–│–┐            3 ┌–––│ · · · · · · · │–––┐
// --       4  │ │ │ · │ · │ · │ │ │ 4  5  6    4 -1 0│ · · · · · · · │1 0│
// --       5  └–│–┘ · └–––┘ · └–│–┘            5 └–––│ · · · · · · · │–––┘
// --       6  · │ · · · · · · · │ ·            6 · · │ · · · · · · · │ · ·
// --       7  ┌–│–┐ · ┌–––┐ · ┌–│–┐            7 · · │ · · · · · · · │ · ·
// --       8  │ └–––––––––––––––┘ │ 7  8  9    8 · ┌–└–––––––––––––––┘–┐ ·
// --       9  └–––┘ · └–––┘ · └–––┘            9 · │0 1│ · ╎0 1╎ · │0 1│ ·
// --      10                                  10 · └–––┘ · └╌╌╌┘ · └–––┘ ·

    ASSERT_THAT(touch(-1,-1,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({-1,-2, 0, -1}))); // 1
    ASSERT_THAT(touch( 3,-1,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({ 3,-2, 0, -1}))); // 2
    ASSERT_THAT(touch( 7,-1,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({ 7,-2, 0, -1}))); // 3
    ASSERT_THAT(touch(-1, 3,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({-2, 3, -1, 0}))); // 4
    ASSERT_THAT(touch( 3, 3,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({ 3, 8,  0, 1}))); // 5
    ASSERT_THAT(touch( 7, 3,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({ 8, 3,  1, 0}))); // 6
    ASSERT_THAT(touch(-1, 7,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({-1, 8,  0, 1}))); // 7
    ASSERT_THAT(touch( 3, 7,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({ 3, 8,  0, 1}))); // 8
    ASSERT_THAT(touch( 7, 7,2,2, 0,0,8,8), testing::ContainerEq(vec2Array({ 7, 8,  0, 1}))); // 9
}

TEST(TOUCH, moving)
{
    ASSERT_THAT(touch(3,3,2,2, 0,0,8,8, 4, 3), testing::ContainerEq(vec2Array({ -2,  3, -1,  0})));
    // ASSERT_THAT(touch(3,3,2,2, 0,0,8,8, 2, 3), testing::ContainerEq(vec2Array({  8,  3,  1,  0})));
    // ASSERT_THAT(touch(3,3,2,2, 0,0,8,8, 2, 3), testing::ContainerEq(vec2Array({  8,  3,  1,  0})));
    // ASSERT_THAT(touch(3,3,2,2, 0,0,8,8, 3, 4), testing::ContainerEq(vec2Array({  3, -2,  0, -1})));
    // ASSERT_THAT(touch(3,3,2,2, 0,0,8,8, 3, 2), testing::ContainerEq(vec2Array({  3,  8,  0,  1})));
}
TEST(TOUCH, tunnels)
{
    ASSERT_THAT(touch(-3, 3,2,2, 0,0,8,8, 3,3), testing::ContainerEq(vec2Array({ -2,  3, -1,  0})));
    ASSERT_THAT(touch( 9, 3,2,2, 0,0,8,8, 3,3), testing::ContainerEq(vec2Array({  8,  3,  1,  0})));
    ASSERT_THAT(touch( 3,-3,2,2, 0,0,8,8, 3,3), testing::ContainerEq(vec2Array({  3, -2,  0, -1})));
    ASSERT_THAT(touch( 3, 9,2,2, 0,0,8,8, 3,3), testing::ContainerEq(vec2Array({  3,  8,  0,  1})));
}

TEST(SLIDE, overlaps)
{
    ASSERT_THAT(slide(3,3,2,2, 0,0,8,8, 4, 5), testing::ContainerEq(vec2Array({ 0.5, -2, 0,-1, 4, -2})));
    ASSERT_THAT(slide(3,3,2,2, 0,0,8,8, 5, 4), testing::ContainerEq(vec2Array({ -2, 0.5, -1,0, -2, 4})));
    ASSERT_THAT(slide(3,3,2,2, 0,0,8,8, 2, 1), testing::ContainerEq(vec2Array({ 5.5, 8, 0,1, 2, 8})));
    ASSERT_THAT(slide(3,3,2,2, 0,0,8,8, 1, 2), testing::ContainerEq(vec2Array({ 8, 5.5, 1,0, 8, 2})));
}

TEST(SLIDE, tunnels)
{
    ASSERT_THAT(slide(10,10,2,2, 0,0,8,8, 1, 4), testing::ContainerEq(vec2Array({ 7, 8, 0, 1, 1, 8})));
    ASSERT_THAT(slide(10,10,2,2, 0,0,8,8, 4, 1), testing::ContainerEq(vec2Array({ 8, 7, 1, 0, 8, 1})));

    //perfect corner case:
    ASSERT_THAT(slide(10,10,2,2, 0,0,8,8, 1, 1), testing::ContainerEq(vec2Array({ 8, 8, 1, 0, 8, 1})));
}

TEST(BOUNCE, overlaps)
{
    ASSERT_THAT(bounce( 3, 3,2,2, 0,0,8,8, 4, 5),  testing::ContainerEq(vec2Array({ 0.5, -2, 0,-1, 4, -9})));
    ASSERT_THAT(bounce( 3, 3,2,2, 0,0,8,8, 5, 4),  testing::ContainerEq(vec2Array({ -2, 0.5, -1,0, -9, 4})));
    ASSERT_THAT(bounce( 3, 3,2,2, 0,0,8,8, 2, 1),  testing::ContainerEq(vec2Array({ 5.5, 8, 0,1, 2, 15})));
    ASSERT_THAT(bounce( 3, 3,2,2, 0,0,8,8, 1, 2),  testing::ContainerEq(vec2Array({ 8, 5.5, 1,0, 15,2})));
}
TEST(BOUNCE, tunnels)
{
    ASSERT_THAT(bounce(10,10,2,2, 0,0,8,8, 1, 4), testing::ContainerEq(vec2Array({ 7, 8, 0, 1, 1, 12})));
    ASSERT_THAT(bounce(10,10,2,2, 0,0,8,8, 4, 1), testing::ContainerEq(vec2Array({ 8, 7, 1, 0, 12, 1})));

    // -- perfect corner case:
    ASSERT_THAT(bounce(10,10,2,2, 0,0,8,8, 1, 1), testing::ContainerEq(vec2Array({ 8, 8, 1, 0, 15, 1})));
}
