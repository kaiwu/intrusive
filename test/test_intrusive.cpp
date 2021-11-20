#include "catch.hpp"
#include "intrusive_bst.h"
#include "intrusive_queue.h"
#include <cstdio>
#include <vector>

bool equal(const std::vector<int32_t>& v1, std::vector<int32_t>&& v2) {
  size_t i = 0;
  while (i < v1.size()) {
    if (v1[i] != v2[i]) {
      return false;
    }
    i++;
  }
  return i == v2.size();
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

  auto collect = [](QUEUE* q, std::vector<int32_t>& v) {
    v.push_back(GETQ(q)->v);
    return true;
  };

  std::vector<int32_t> v1;
  q1.iterate(collect, std::ref(v1));
  REQUIRE(equal(v1, {0, 1, 2, 3}));

  std::vector<int32_t> v2;
  q1.iterate_r(collect, std::ref(v2));
  REQUIRE(equal(v2, {3, 2, 1, 0}));

  auto find1 = [](QUEUE* q) { return GETQ(q)->v == 1; };
  QUEUE* p = q1.find(find1);
  REQUIRE(GETQ(p)->v == 1);
  REQUIRE(p == &qs1[1].link);
  q1.dequeue(p);
  REQUIRE(q1.size() == 3);
  p = q1.find_r(find1);
  REQUIRE(p == nullptr);

  std::vector<int32_t> v3;
  q1.iterate(collect, std::ref(v3));
  REQUIRE(equal(v3, {0, 2, 3}));

  Q x1{1};
  auto find2 = [](QUEUE* q) { return GETQ(q)->v == 2; };
  q1.insert_before(&x1.link, find2);
  REQUIRE(q1.size() == 4);
  std::vector<int32_t> v4;
  q1.iterate(collect, std::ref(v4));
  REQUIRE(equal(v4, {0, 1, 2, 3}));
  p = q1.find_r(find1);
  REQUIRE(p == &x1.link);

  Q x10{10};
  auto find10 = [](QUEUE* q) { return GETQ(q)->v == 10; };
  p = q1.insert_after(&x10.link, find10);
  REQUIRE(p == nullptr);
  std::vector<int32_t> v5;
  q1.iterate(collect, std::ref(v5));
  REQUIRE(equal(v5, {0, 1, 2, 3}));

  Q qs2[] = {{4}, {5}, {6}, {7}};
  intrusive_queue q2;
  for (auto& q : qs2) {
    q2.enqueue_back(&q.link);
  }
  q1.append(q2); // q2 cannot be used
  REQUIRE(q1.size() == 8);

  intrusive_queue q3;
  q1.split(find2, q3);
  REQUIRE(q1.size() == 2);
  REQUIRE(q3.size() == 6);
  std::vector<int32_t> v6;
  std::vector<int32_t> v7;
  q1.iterate(collect, std::ref(v6));
  REQUIRE(equal(v6, {0, 1}));
  q3.iterate(collect, std::ref(v7));
  REQUIRE(equal(v7, {2, 3, 4, 5, 6, 7}));

  std::vector<int32_t> v8;
  while (!q3.empty()) {
    QUEUE* q = q3.front();
    v8.push_back(GETQ(q)->v);
    q3.dequeue(q);
  }
  REQUIRE(q3.size() == 0);
  REQUIRE(equal(v8, {2, 3, 4, 5, 6, 7}));

  intrusive_queue q4;
  q1.move(q4);
  REQUIRE(q1.empty());
  REQUIRE(q4.size() == 2);
  std::vector<int32_t> v9;
  q4.iterate(collect, std::ref(v9));
  REQUIRE(equal(v9, {0, 1}));
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
