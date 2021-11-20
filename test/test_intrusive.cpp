#include "catch.hpp"
#include "intrusive_bst.h"
#include "intrusive_queue.h"
#include <vector>

bool equal(const std::vector<int32_t>& v1, std::vector<int32_t>&& v2) {
  if (v1.size() != v2.size())
    return false;
  for (size_t i = 0; i < v1.size(); i++) {
    if (v1[i] != v2[i])
      return false;
  }
  return true;
}

struct Q {
  int32_t v;
  QUEUE link;
  Q(int32_t i) : v(i) {
    QUEUE_INIT(&link);
  }
};

#define GETQ(x) QUEUE_DATA(x, Q, link)

TEST_CASE("intrusive queue", "[]") {
  Q qs1[] = {{0}, {1}, {2}, {3}};
  intrusive_queue q1;
  REQUIRE(q1.empty());
  q1.enqueue_back(&qs1[1].link);
  q1.enqueue_back(&qs1[2].link);
  q1.enqueue_back(&qs1[3].link);
  q1.enqueue_front(&qs1[0].link);
  REQUIRE(!q1.empty());
  REQUIRE(q1.size() == 4);
  REQUIRE(GETQ(q1.front())->v == 0);

  std::vector<int32_t> v1;
  q1.iterate([&v1](QUEUE* q) -> bool {
    v1.push_back(GETQ(q)->v);
    return true;
  });
  REQUIRE(equal(v1, std::vector<int32_t>{0, 1, 2, 3}));

  std::vector<int32_t> v2;
  q1.iterate_r([&v2](QUEUE* q) -> bool {
    v2.push_back(GETQ(q)->v);
    return true;
  });
  REQUIRE(equal(v2, std::vector<int32_t>{3, 2, 1, 0}));

  QUEUE* p = q1.find([](QUEUE* q) { return GETQ(q)->v == 1; });
  REQUIRE(GETQ(p)->v == 1);
  q1.dequeue(p);
  REQUIRE(q1.size() == 3);
  p = q1.find_r([](QUEUE* q) { return GETQ(q)->v == 1; });
  REQUIRE(p == nullptr);

  std::vector<int32_t> v3;
  q1.iterate([&v3](QUEUE* q) -> bool {
    v3.push_back(GETQ(q)->v);
    return true;
  });
  REQUIRE(equal(v3, std::vector<int32_t>{0, 2, 3}));

  Q x1{1};
  q1.insert_before(&x1.link,[](QUEUE* q) { return GETQ(q)->v == 2; });
  REQUIRE(q1.size() == 4);
  std::vector<int32_t> v4;
  q1.iterate([&v4](QUEUE* q) -> bool {
    v4.push_back(GETQ(q)->v);
    return true;
  });
  REQUIRE(equal(v4, std::vector<int32_t>{0, 1, 2, 3}));
  p = q1.find_r([](QUEUE* q) { return GETQ(q)->v == 1; });
  REQUIRE(p == &x1.link);

  // Q qs2[] = {{4}, {5}, {6}, {7}};
}

struct T {
  int32_t v;
  TREE node;
  T(int32_t i) : v(i) {
    TREE_INIT(&node);
  }
};

#define GETT(x) TREE_DATA(x, T, node)

TEST_CASE("intrusive bst", "[]") {
  REQUIRE(1 == 1);
}
