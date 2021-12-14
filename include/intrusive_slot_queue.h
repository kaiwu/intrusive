#pragma once
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

constexpr uint32_t npos = UINT32_MAX;
typedef uint32_t SLOT_QUEUE[3];

template <typename T>
T *address(uint32_t slot);
template <typename T>
SLOT_QUEUE *address(T *);

#define ADDRESS(T, s) (address<T>(address<T>(s)))
#define SLOT_QUEUE_DATA(ptr, type, field) \
  ((type *)((char *)(ptr)-offsetof(type, field)))

#define SLOT_QUEUE_SLOT(q) (*(uint32_t *)&((*(q))[0]))
#define SLOT_QUEUE_NEXT(q) (*(uint32_t *)&((*(q))[1]))
#define SLOT_QUEUE_PREV(q) (*(uint32_t *)&((*(q))[2]))

#define SLOT_QUEUE_INIT(q, s)    \
  do {                           \
    SLOT_QUEUE_SLOT(q) = (s);    \
    SLOT_QUEUE_NEXT(q) = (npos); \
    SLOT_QUEUE_PREV(q) = (npos); \
  } while (0)

#define SLOT_QUEUE_EMPTY(q) (SLOT_QUEUE_SLOT(q) == SLOT_QUEUE_NEXT(q))

template <typename T>
struct intrusive_slot_queue {
 private:
  SLOT_QUEUE head;

  void enqueue_after(SLOT_QUEUE *tq, SLOT_QUEUE *xq) {
    uint32_t n = SLOT_QUEUE_NEXT(xq);
    SLOT_QUEUE *nq = (n == npos ? &head : ADDRESS(T, n));
    assert(nq != nullptr);

    SLOT_QUEUE_NEXT(xq) = SLOT_QUEUE_SLOT(tq);
    SLOT_QUEUE_PREV(tq) = SLOT_QUEUE_SLOT(xq);
    SLOT_QUEUE_NEXT(tq) = SLOT_QUEUE_SLOT(nq);
    SLOT_QUEUE_PREV(nq) = SLOT_QUEUE_SLOT(tq);
  }

  void enqueue_before(SLOT_QUEUE *tq, SLOT_QUEUE *xq) {
    uint32_t p = SLOT_QUEUE_PREV(xq);
    SLOT_QUEUE *pq = (p == npos ? &head : ADDRESS(T, p));
    assert(pq != nullptr);

    SLOT_QUEUE_PREV(xq) = SLOT_QUEUE_SLOT(tq);
    SLOT_QUEUE_NEXT(tq) = SLOT_QUEUE_SLOT(xq);
    SLOT_QUEUE_PREV(tq) = SLOT_QUEUE_SLOT(pq);
    SLOT_QUEUE_NEXT(pq) = SLOT_QUEUE_SLOT(tq);
  }

 public:
  intrusive_slot_queue() { SLOT_QUEUE_INIT(&head, npos); }

  bool empty() const noexcept { return SLOT_QUEUE_EMPTY(&head); }
  T *front() const noexcept { return address<T>(SLOT_QUEUE_NEXT(&head)); }

  void enqueue_front(T *t) { enqueue_after(address<T>(t), &head); }
  void enqueue_back(T *t) { enqueue_before(address<T>(t), &head); }

  void dequeue(T *t) {
    SLOT_QUEUE *s = address<T>(t);
    uint32_t n = SLOT_QUEUE_NEXT(s);
    uint32_t p = SLOT_QUEUE_PREV(s);

    do {
      if (n == npos && p == npos) {  // t is head and tail
        SLOT_QUEUE_NEXT(&head) = SLOT_QUEUE_SLOT(&head);
        SLOT_QUEUE_PREV(&head) = SLOT_QUEUE_SLOT(&head);
        break;
      }

      if (n != npos && p == npos) {  // t is head
        SLOT_QUEUE *nq = ADDRESS(T, n);
        SLOT_QUEUE_NEXT(&head) = SLOT_QUEUE_SLOT(nq);
        SLOT_QUEUE_PREV(nq) = SLOT_QUEUE_SLOT(&head);
        break;
      }

      if (n == npos && p != npos) {  // t is tail
        SLOT_QUEUE *pq = ADDRESS(T, p);
        SLOT_QUEUE_NEXT(pq) = SLOT_QUEUE_SLOT(&head);
        SLOT_QUEUE_PREV(&head) = SLOT_QUEUE_SLOT(pq);
        break;
      }

      if (n != npos && p != npos) {  // t is middle
        SLOT_QUEUE *nq = ADDRESS(T, n);
        SLOT_QUEUE *pq = ADDRESS(T, p);
        SLOT_QUEUE_NEXT(pq) = SLOT_QUEUE_SLOT(nq);
        SLOT_QUEUE_PREV(nq) = SLOT_QUEUE_SLOT(pq);
        break;
      }
    } while (false);

    uint32_t slot = SLOT_QUEUE_SLOT(s);
    SLOT_QUEUE_INIT(s, slot);
  }

  template <typename F, typename... Args>
  void iterate(F &&f, Args &&...args) const noexcept {
    SLOT_QUEUE *n = ADDRESS(T, SLOT_QUEUE_NEXT(&head));
    while (n != nullptr && SLOT_QUEUE_SLOT(n) != SLOT_QUEUE_SLOT(&head)) {
      if (!f(address<T>(SLOT_QUEUE_SLOT(n)), args...)) {
        break;
      }
      n = ADDRESS(T, SLOT_QUEUE_NEXT(n));
    }
  }

  template <typename F, typename... Args>
  void iterate_r(F &&f, Args &&...args) const noexcept {
    SLOT_QUEUE *p = ADDRESS(T, SLOT_QUEUE_PREV(&head));
    while (p != nullptr && SLOT_QUEUE_SLOT(p) != SLOT_QUEUE_SLOT(&head)) {
      if (!f(address<T>(SLOT_QUEUE_SLOT(p)), args...)) {
        break;
      }
      p = ADDRESS(T, SLOT_QUEUE_PREV(p));
    }
  }

  size_t size() const noexcept {
    size_t count = 0;
    iterate([&count](T *p) -> bool {
      count++;
      return true;
    });
    return count;
  }

  template <typename F>
  T *find(F &&f) {
    T *p = nullptr;
    iterate([&p, f](T *x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F>
  T *find_r(F &&f) {
    T *p = nullptr;
    iterate_r([&p, f](T *x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F>
  T *insert_before(T *t, F &&f) {
    T *x = find(f);
    if (x != nullptr) {
      enqueue_before(address<T>(t), address<T>(x));
    }
    return x;
  }

  template <typename F>
  T *insert_after(T *t, F &&f) {
    T *x = find(f);
    if (x != nullptr) {
      enqueue_after(address<T>(t), address<T>(x));
    }
    return x;
  }
};
