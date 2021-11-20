#pragma once
#include <deque>
#include <stddef.h>

typedef void* TREE[2];
#define TREE_LEFT(t) (*(TREE**) &((*(t))[0]))
#define TREE_RIGHT(t) (*(TREE**) &((*(t))[1]))

#define TREE_INIT(t)                                                           \
  do {                                                                         \
    TREE_LEFT(t) = (t);                                                        \
    TREE_RIGHT(t) = (t);                                                       \
  } while (0)

#define TREE_DATA(ptr, type, field)                                            \
  ((type*) ((char*) (ptr) -offsetof(type, field)))

#define LEFT_EMPTY(t) ((const TREE*) (t) == (const TREE*) TREE_LEFT(t))
#define RIGHT_EMPTY(t) ((const TREE*) (t) == (const TREE*) TREE_RIGHT(t))
#define TREE_EMPTY(t) (LEFT_EMPTY(t) && RIGHT_EMPTY(t))

typedef bool (*TREE_LESS_T)(TREE*, TREE*);
typedef enum {
  TREE_TRAVERSE_PREORDER,
  TREE_TRAVERSE_INORDER,
  TREE_TRAVERSE_POSTORDER,
  TREE_TRAVERSE_BFS,
} tree_traverse_t;

struct intrusive_bst {
  TREE* root;

  intrusive_bst() : root(nullptr) {
  }

  bool empty() const noexcept {
    return root == nullptr;
  }

  void clear() {
    root = nullptr;
  }

  void insert(TREE* r, TREE* t, TREE_LESS_T compare) {
    if (compare(t, r)) {
      if (LEFT_EMPTY(r)) {
        TREE_LEFT(r) = t;
        return;
      }
      insert(TREE_LEFT(r), t, compare);
    } else {
      if (RIGHT_EMPTY(r)) {
        TREE_RIGHT(r) = t;
        return;
      }
      insert(TREE_RIGHT(r), t, compare);
    }
  }

  void insert(TREE* t, TREE_LESS_T compare) {
    if (!empty()) {
      insert(root, t, compare);
    } else {
      root = t;
    }
  }

  void erase(TREE** r, TREE* p, TREE* t, TREE_LESS_T compare) {
    bool less = compare(t, *r);
    if (less && !LEFT_EMPTY(*r)) {
      erase(&TREE_LEFT(*r), *r, t, compare);
    }
    bool greater = compare(*r, t);
    if (greater && !RIGHT_EMPTY(*r)) {
      erase(&TREE_RIGHT(*r), *r, t, compare);
    }

    if (!less && !greater) {
      std::deque<TREE*> ts;
      auto collect = [&ts](TREE* t) { ts.push_back(t); };
      if (!LEFT_EMPTY(*r)) {
        iterate_postorder(collect, TREE_LEFT(*r));
      }
      if (!RIGHT_EMPTY(*r)) {
        iterate_postorder(collect, TREE_RIGHT(*r));
      }
      if (ts.size() > 0) {
        intrusive_bst sub;
        for (TREE* x : ts) {
          TREE_INIT(x);
          sub.insert(x, compare);
        }
        *r = sub.root;
      } else {
        *r = p;
      }
    }
  }

  void erase(TREE* t, TREE_LESS_T compare) {
    erase(&root, root, t, compare);
  }

  TREE* min(TREE* r) const noexcept {
    TREE* t = r;
    while (t != nullptr && !LEFT_EMPTY(t)) {
      t = TREE_LEFT(t);
    }
    return t;
  }

  TREE* max(TREE* r) const noexcept {
    TREE* t = r;
    while (t != nullptr && !RIGHT_EMPTY(t)) {
      t = TREE_RIGHT(t);
    }
    return t;
  }

  template <typename F, typename... Args>
  void iterate_preorder(F&& f, TREE* last, Args&&... args) const noexcept {
    f(last, args...);
    if (!LEFT_EMPTY(last))
      iterate_preorder(f, TREE_LEFT(last), args...);
    if (!RIGHT_EMPTY(last))
      iterate_preorder(f, TREE_RIGHT(last), args...);
  }

  template <typename F, typename... Args>
  void iterate_inorder(F&& f, TREE* last, Args&&... args) const noexcept {
    if (!LEFT_EMPTY(last))
      iterate_inorder(f, TREE_LEFT(last), args...);
    f(last, args...);
    if (!RIGHT_EMPTY(last))
      iterate_inorder(f, TREE_RIGHT(last), args...);
  }

  template <typename F, typename... Args>
  void iterate_postorder(F&& f, TREE* last, Args&&... args) const noexcept {
    if (!LEFT_EMPTY(last))
      iterate_postorder(f, TREE_LEFT(last), args...);
    if (!RIGHT_EMPTY(last))
      iterate_postorder(f, TREE_RIGHT(last), args...);
    f(last, args...);
  }

  template <typename F, typename... Args>
  void iterate_bfs(F&& f, TREE* last, Args&&... args) const noexcept {
    std::deque<TREE*> ts;
    ts.push_back(last);
    while (!ts.empty()) {
      TREE* t = ts.front();
      ts.pop_front();
      f(t, args...);
      if (!LEFT_EMPTY(t))
        ts.push_back(TREE_LEFT(t));
      if (!RIGHT_EMPTY(t))
        ts.push_back(TREE_RIGHT(t));
    }
  }

  template <typename F, typename... Args>
  void iterate(tree_traverse_t traverse, F&& f, Args&&... args) const noexcept {
    if (!empty()) {
      switch (traverse) {
        case TREE_TRAVERSE_PREORDER:
          iterate_preorder(f, root, args...);
          break;
        case TREE_TRAVERSE_INORDER:
          iterate_inorder(f, root, args...);
          break;
        case TREE_TRAVERSE_POSTORDER:
          iterate_postorder(f, root, args...);
          break;
        case TREE_TRAVERSE_BFS:
          iterate_bfs(f, root, args...);
          break;
      }
    }
  }

  size_t size() const noexcept {
    size_t size = 0;
    auto count = [&size](TREE* t) { size++; };
    iterate(TREE_TRAVERSE_INORDER, count);
    return size;
  }

  size_t height(TREE* t) const noexcept {
    size_t l = LEFT_EMPTY(t) ? 0 : height(TREE_LEFT(t));
    size_t r = RIGHT_EMPTY(t) ? 0 : height(TREE_RIGHT(t));
    return 1 + (l > r ? l : r);
  }

  size_t height() const noexcept {
    return empty() ? 0ll : height(root);
  }
};
