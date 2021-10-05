#include "../World.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace plugin::physics::bump;

class TestItem
{
public:
    TestItem(){}
    std::string name="untitled";
};


static void CompareSet(std::set<Item*> a, std::set<Item*> b)
{
    std::set<std::string> setA;
    for (auto item: a)
    {
        setA.insert(item->UserData.Get<TestItem>()->name);
    }

    std::set<std::string> setB;
    for (auto item: b)
    {
        setB.insert(item->UserData.Get<TestItem>()->name);
    }

    ASSERT_THAT(setA, testing::ContainerEq(setB));
}
static void CompareVec2(math::vec2 a, math::vec2 b)
{
    ASSERT_EQ(a.x, b.x);
    ASSERT_EQ(a.y, b.y);
}
static void CompareRectangle(Rectangle first, Rectangle other)
{
    CompareVec2(first.pos, other.pos);
    CompareVec2(first.scale, other.scale);
}



class BumpWorldTest : public ::testing::Test {
protected:
    World world;
  BumpWorldTest() {

  }

  virtual ~BumpWorldTest() {

  }
  void QueryPointTest(int x,int y, std::set<Item*> expected)
  {
      std::set<Item*> items;
      world.QueryPoint(items, math::vec2(x,y));
      CompareSet(items, expected);
  }
  void QueryPointTest(int x,int y, FilterSingle filter, std::set<Item*> expected)
  {
      std::set<Item*> items;
      world.QueryPoint(items, math::vec2(x,y), filter);
      CompareSet(items, expected);
  }

  void QuerySegmentTest(float x, float y , float x2, float y2, std::set<Item*> expected)
  {
      std::set<Item*> items;
      world.QuerySegment(items, math::vec2(x,y), math::vec2(x2,y2));
      CompareSet(items, expected);
  }
  void QuerySegmentTest(float x, float y , float x2, float y2, FilterSingle filter, std::set<Item*> expected)
  {
      std::set<Item*> items;
      world.QuerySegment(items, math::vec2(x,y), math::vec2(x2,y2), filter);
      CompareSet(items, expected);
  }
  virtual Item* AddItem(std::string name, float x,float y, float w,float h)
  {
      auto itemCore = std::make_shared<TestItem>();
      itemCore->name = name;
      return world.Add(itemCore, Rectangle(x,y,w,h));
  }
  virtual Item* AddItem(float x,float y, float w,float h)
  {
    return AddItem("untitled", x,y,w,h);
  }

  virtual void SetUp() {
      world = World();
  }

  virtual void TearDown() {

  }


};
