#include <cstdio>
#include <vector>

#include "catch.hpp"
#include "intrusive_bst.h"
#include "intrusive_queue.h"
#include "intrusive_slot_queue.h"

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
  Q(int32_t i) : v(i) { QUEUE_INIT(&link); }
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

  std::vector<int32_t> v0;
  for (auto it = q1.begin(); it != q1.end(); it++) {
    v0.push_back(GETQ(*it)->v);
  }
  REQUIRE(equal(v0, {0, 1, 2, 3}));

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
  for (auto& q : qs2) { // auto& intrusive !!!
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
  T(int32_t i) : v(i) { TREE_INIT(&node); }
};

#define GETT(x) TREE_DATA(x, T, node)

bool compareT(TREE* t1, TREE* t2) { return GETT(t1)->v < GETT(t2)->v; }

TEST_CASE("intrusive bst", "[]") {
  T ts[] = {{4}, {2}, {1}, {3}, {0}, {5}};
  intrusive_bst bst;
  REQUIRE(bst.empty());

  for (auto& t : ts) {
    bst.insert(&t.node, compareT);
  }
  REQUIRE(!bst.empty());
  REQUIRE(bst.size() == 6);

  auto collect = [](TREE* t, std::vector<int32_t>& v) {
    v.push_back(GETT(t)->v);
    return true;
  };

  std::vector<int32_t> preorder;
  bst.iterate(TREE_TRAVERSE_PREORDER, collect, std::ref(preorder));
  REQUIRE(equal(preorder, {4, 2, 1, 0, 3, 5}));

  std::vector<int32_t> inorder;
  bst.iterate(TREE_TRAVERSE_INORDER, collect, std::ref(inorder));
  REQUIRE(equal(inorder, {0, 1, 2, 3, 4, 5}));

  std::vector<int32_t> postorder;
  bst.iterate(TREE_TRAVERSE_POSTORDER, collect, std::ref(postorder));
  REQUIRE(equal(postorder, {0, 1, 3, 2, 5, 4}));

  std::vector<int32_t> bfs;
  bst.iterate(TREE_TRAVERSE_BFS, collect, std::ref(bfs));
  REQUIRE(equal(bfs, {4, 2, 5, 1, 3, 0}));

  TREE* min = bst.min(bst.root);
  REQUIRE(GETT(min)->v == 0);

  TREE* max = bst.max(bst.root);
  REQUIRE(GETT(max)->v == 5);

  REQUIRE(bst.height() == 4);
  REQUIRE(bst.height(min) == 1);
  REQUIRE(bst.height(max) == 1);

  T t1{1}, t30{30};
  bst.erase(&t1.node, compareT);
  REQUIRE(bst.height() == 3);
  REQUIRE(bst.size() == 5);
  inorder.clear();
  bst.iterate(TREE_TRAVERSE_INORDER, collect, std::ref(inorder));
  REQUIRE(equal(inorder, {0, 2, 3, 4, 5}));

  bfs.clear();
  bst.iterate(TREE_TRAVERSE_BFS, collect, std::ref(bfs));
  REQUIRE(equal(bfs, {4, 2, 5, 0, 3}));

  bst.erase(&t30.node, compareT);
  REQUIRE(bst.size() == 5);

  bst.clear();
  REQUIRE(bst.empty());
}

struct SQ {
  uint32_t v;
  SLOT_QUEUE q;

  SQ(uint32_t i) : v(i) { SLOT_QUEUE_INIT(&q, i); }
};

template <> SLOT_QUEUE* address<SQ>(SQ* sq) { return sq == nullptr ? nullptr : &(sq->q); }

template <> SQ* address<SQ>(uint32_t slot) {
  static std::vector<SQ> q{{0}, {1}, {2}, {3}, {4}};
  if (slot < q.size()) {
    return &q[slot];
  }
  return nullptr;
}

TEST_CASE("intrusive slot queue", "[]") {
  intrusive_slot_queue<SQ> q;
  q.enqueue_back(address<SQ>(1u));
  q.enqueue_back(address<SQ>(2u));
  q.enqueue_back(address<SQ>(3u));
  q.enqueue_front(address<SQ>(0u));

  REQUIRE(!q.empty());
  REQUIRE(q.size() == 4);
  REQUIRE(q.front()->v == 0);

  auto collect = [](SQ* q, std::vector<int32_t>& v) {
    v.push_back(q->v);
    return true;
  };
  std::vector<int32_t> v1;
  q.iterate(collect, std::ref(v1));
  REQUIRE(equal(v1, {0, 1, 2, 3}));

  std::vector<int32_t> v2;
  q.iterate_r(collect, std::ref(v2));
  REQUIRE(equal(v2, {3, 2, 1, 0}));

  auto find1 = [](SQ* q) { return q->v == 1; };
  SQ* p = q.find(find1);
  REQUIRE(p->v == 1);
  REQUIRE(p == address<SQ>(1u));
  q.dequeue(p);
  REQUIRE(q.size() == 3);
  p = q.find_r(find1);
  REQUIRE(p == nullptr);

  std::vector<int32_t> v3;
  q.iterate(collect, std::ref(v3));
  REQUIRE(equal(v3, {0, 2, 3}));

  auto find2 = [](SQ* q) { return q->v == 3; };
  q.insert_before(address<SQ>(1u), find2);
  REQUIRE(q.size() == 4);
  std::vector<int32_t> v4;
  q.iterate(collect, std::ref(v4));
  REQUIRE(equal(v4, {0, 2, 1, 3}));
  p = q.find_r(find1);
  REQUIRE(p == address<SQ>(1u));

  auto find10 = [](SQ* q) { return q->v == 10; };
  p = q.insert_after(address<SQ>(4u), find10);
  REQUIRE(p == nullptr);
  std::vector<int32_t> v5;
  q.iterate(collect, std::ref(v5));
  REQUIRE(equal(v5, {0, 2, 1, 3}));

  std::vector<int32_t> v6;
  while (!q.empty()) {
    SQ* x = q.front();
    v6.push_back(x->v);
    q.dequeue(x);
  }
  REQUIRE(q.size() == 0);
  REQUIRE(equal(v6, {0, 2, 1, 3}));
}
