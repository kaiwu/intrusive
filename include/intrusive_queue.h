#pragma once
#include "queue.h"

#define QUEUE_FOREACH_R(q, h)                                                  \
  for ((q) = QUEUE_PREV(h); (q) != (h); (q) = QUEUE_PREV(q))

struct intrusive_queue {
  struct iterator {
    QUEUE* next;

    bool operator==(const iterator& i) const noexcept {
      return next == i.next;
    }
    bool operator!=(const iterator& i) const noexcept {
      return next != i.next;
    }

    QUEUE* operator*() noexcept {
      return next;
    }

    iterator& operator++() noexcept {
      next = QUEUE_NEXT(next);
      return *this;
    }

    iterator operator++(int) noexcept {
      iterator t = *this;
      ++(*this);
      return t;
    }
  };

  QUEUE head;
  intrusive_queue() {
    QUEUE_INIT(&head);
  }

  bool empty() const noexcept {
    return QUEUE_EMPTY(&head);
  }

  iterator begin() noexcept {
    return {QUEUE_HEAD(&head)};
  }

  iterator end() noexcept {
    return {&head};
  }

  QUEUE* front() const noexcept {
    return QUEUE_HEAD(&head);
  }

  void enqueue_back(QUEUE* p) {
    QUEUE_INSERT_TAIL(&head, p);
  }

  void enqueue_front(QUEUE* p) {
    QUEUE_INSERT_HEAD(&head, p);
  }

  void dequeue(QUEUE* p) {
    QUEUE_REMOVE(p);
  }

  template <typename F, typename... Args>
  void iterate(F&& f, Args&&... args) {
    QUEUE* p = nullptr;
    QUEUE_FOREACH(p, &head) {
      if (!f(p, args...))
        break;
    }
  }

  template <typename F, typename... Args>
  void iterate_r(F&& f, Args&&... args) {
    QUEUE* p = nullptr;
    QUEUE_FOREACH_R(p, &head) {
      if (!f(p, args...))
        break;
    }
  }

  // O(n)
  size_t size() {
    size_t count = 0;
    iterate([&count](QUEUE* p) -> bool {
      count++;
      return true;
    });
    return count;
  }

  template <typename F>
  QUEUE* find(F&& f) {
    QUEUE* p = nullptr;
    iterate([&p, f](QUEUE* x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F>
  QUEUE* find_r(F&& f) {
    QUEUE* p = nullptr;
    iterate_r([&p, f](QUEUE* x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F>
  QUEUE* insert_before(QUEUE* p, F&& f) {
    QUEUE* x = find(f);
    if (x != nullptr) {
      QUEUE_INSERT_TAIL(x, p);
    }
    return x;
  }

  template <typename F>
  QUEUE* insert_after(QUEUE* p, F&& f) {
    QUEUE* x = find(f);
    if (x != nullptr) {
      QUEUE_INSERT_HEAD(x, p);
    }
    return x;
  }

  void append(intrusive_queue& q) {
    QUEUE_ADD(&head, &q.head);
  }

  template <typename F>
  void split(F&& f, intrusive_queue& q) {
    QUEUE* x = find(f);
    if (x != nullptr) {
      QUEUE_SPLIT(&head, x, &q.head);
    }
  }

  void move(intrusive_queue& q) {
    if (!empty()) {
      split([](QUEUE* x) { return true; }, q);
    }
  }
};
