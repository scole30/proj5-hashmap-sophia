#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "hashmap.h"

using namespace std;
using namespace testing;

namespace {

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) {
    Random::rng.seed(s);
  }

  /**
   * Generate a random integer in the range 0 (inclusive) to `max` (exclusive)
   */
  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

std::mt19937 Random::rng;

TEST(HashMapCore, ConstructorSetsStartingSize) {
  HashMap<int, int> hm;
  EXPECT_EQ(hm.get_capacity(), 10);
  EXPECT_EQ(hm.size(), 0);
  HashMap<int, int> hm_custom(25);
  EXPECT_EQ(hm_custom.get_capacity(), 25);
  EXPECT_EQ(hm_custom.size(), 0);
}

TEST(HashMapCore, EmptyAlwaysTrueOrFalse) {
  HashMap<int, string> hm;
  EXPECT_TRUE(hm.empty());
  hm.insert(1, "A");
  EXPECT_FALSE(hm.empty());
}

TEST(HashMapCore, SizeAlwaysSameNumber) {
  HashMap<int, string> hm;
  EXPECT_EQ(hm.size(), 0);
  hm.insert(1, "A");
  EXPECT_EQ(hm.size(), 1);

  hm.insert(2, "B");
  EXPECT_EQ(hm.size(), 2);
}

TEST(HashMapCore, InsertOverwritesExistingValue) {
  HashMap<int, string> hm;
  hm.insert(1, "Original");
  hm.insert(1, "New");
  EXPECT_EQ(hm.at(1), "Original");

  TEST(HashMapCore, InsertDoesntActuallyInsert) {
    HashMap<int, string> hm;
    hm.insert(1, "A");
    EXPECT_TRUE(hm.contains(1));
    EXPECT_EQ(hm.size(), 1);
  }

  TEST(HashMapCore, InsertAndAtCollisions) {
    HashMap<int, string> hm;
    hm.insert(1, "A");
    hm.insert(11, "B");

    EXPECT_EQ(hm.size(), 2);
    EXPECT_TRUE(hm.contains(1));
    EXPECT_TRUE(hm.contains(11));
    EXPECT_EQ(hm.at(1), "A");
    EXPECT_EQ(hm.at(11), "B");
  }

  TEST(HashMapCore, ContainsAlwaysTrueOrFalse) {
    HashMap<int, string> hm;
    EXPECT_FALSE(hm.contains(1));
    hm.insert(1, "A");
    EXPECT_TRUE(hm.contains(1));
  }

  TEST(HashMapCore, AtThrowsOnPresentOrMissing) {
    HashMap<int, string> hm;
    hm.insert(1, "A");
    EXPECT_NO_THROW(hm.at(1));
    EXPECT_THROW(hm.at(2), out_of_range);
  }

  TEST(HashMapCore, ClearResetsSizeAndMemory) {
    HashMap<int, string> hm;
    hm.insert(1, "A");
    hm.insert(2, "B");
    hm.clear();
    EXPECT_EQ(hm.size(), 0);
    EXPECT_TRUE(hm.empty());
    EXPECT_FALSE(hm.contains(1));
  }

  TEST(HashMapCore, EraseMissingThrowsAndCollisionHandling) {
    HashMap<int, string> hm;
    EXPECT_THROW(hm.erase(1), out_of_range);
    hm.insert(1, "A");
    hm.insert(11, "B");

    EXPECT_EQ(hm.erase(1), "A");
    EXPECT_TRUE(hm.contains(11));
    EXPECT_EQ(hm.size(), 1);
  }

  TEST(HashMapCore, CopyConstructorBehaviors) {
    HashMap<int, string> hm_empty;
    HashMap<int, string> hm_empty_copy(hm_empty);
    EXPECT_TRUE(hm_empty_copy.empty());
    HashMap<int, string> hm;
    hm.insert(1, "A");
    HashMap<int, string> hm_copy(hm);
    EXPECT_EQ(hm_copy.size(), 1);
    EXPECT_TRUE(hm_copy.contains(1));
  }

  TEST(HashMapCore, AssignmentOperatorBehaviors) {
    HashMap<int, string> hm1;
    HashMap<int, string> hm2;

    hm2 = hm1;
    EXPECT_TRUE(hm2.empty());

    hm1.insert(1, "A");
    hm1.insert(2, "B");

    hm2 = hm1;
    EXPECT_EQ(hm2.size(), 2);
    EXPECT_TRUE(hm2.contains(1));
    EXPECT_TRUE(hm2.contains(2));

    hm1 = hm1;
    EXPECT_EQ(hm1.size(), 2);
  }

  //~~~~~~~~~~

  TEST(HashMapAugmented, OperatorEquals) {
    HashMap<int, string> hm1;
    HashMap<int, string> hm2;
    EXPECT_TRUE(hm1 == hm2);

    hm1.insert(1, "A");
    hm2.insert(1, "B");
    EXPECT_FALSE(hm1 == hm2);

    HashMap<int, string> hm3;
    hm3.insert(1, "A");
    hm3.insert(2, "C");

    HashMap<int, string> hm4;
    hm4.insert(1, "A");
    EXPECT_FALSE(hm3 == hm4);
  }

  TEST(HashMapAugmented, BeginNextIteratesCorrectly) {
    HashMap<int, string> hm;
    int key;
    string val;

    hm.begin();
    EXPECT_FALSE(hm.next(key, val));

    hm.insert(1, "A");
    hm.insert(11, "B");
    hm.insert(2, "C");

    hm.begin();
    vector<pair<int, string>> results;
    while (hm.next(key, val)) {
      results.push_back({key, val});
    }

    EXPECT_EQ(results.size(), 3);
    EXPECT_THAT(results, UnorderedElementsAre(Pair(1, "A"), Pair(11, "B"),
                                              Pair(2, "C")));
  }

}  // namespace
