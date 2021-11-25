#pragma once
#include <stddef.h>
#include <stdint.h>

constexpr uint32_t npos = UINT32_MAX;
typedef uint32_t SLOT_QUEUE[3];

template <typename T> T *address(uint32_t slot);
template <typename T> SLOT_QUEUE *address(T *);

#define SLOT_QUEUE_DATA(ptr, type, field)                                      \
  ((type *)((char *)(ptr)-offsetof(type, field)))

#define SLOT_QUEUE_SLOT(q) (*(uint32_t *)&((*(q))[0]))
#define SLOT_QUEUE_NEXT(q) (*(uint32_t *)&((*(q))[1]))
#define SLOT_QUEUE_PREV(q) (*(uint32_t *)&((*(q))[2]))

#define SLOT_QUEUE_INIT(q, s)                                                  \
  do {                                                                         \
    SLOT_QUEUE_SLOT(q) = (s);                                                  \
    SLOT_QUEUE_NEXT(q) = (s);                                                  \
    SLOT_QUEUE_PREV(q) = (s);                                                  \
  } while (0)

#define SLOT_QUEUE_EMPTY(q) (SLOT_QUEUE_SLOT(q) == SLOT_QUEUE_NEXT(q))

template <typename T> struct intrusive_slot_queue {
private:
  SLOT_QUEUE head;

public:
  intrusive_slot_queue() { SLOT_QUEUE_INIT(&head, npos); }

  bool empty() const noexcept { return SLOT_QUEUE_EMPTY(&head); }
  T *front() const noexcept { return address<T>(SLOT_QUEUE_NEXT(&head)); }

  void enqueue_back(T *t) {
    SLOT_QUEUE *s = address<T>(t);
    if (empty()) {
      SLOT_QUEUE_NEXT(&head) = SLOT_QUEUE_SLOT(s);
      SLOT_QUEUE_PREV(&head) = SLOT_QUEUE_SLOT(s);
      SLOT_QUEUE_NEXT(s) = SLOT_QUEUE_SLOT(&head);
      SLOT_QUEUE_PREV(s) = SLOT_QUEUE_SLOT(&head);
    } else {
      T *last = address<T>(SLOT_QUEUE_PREV(&head));
      SLOT_QUEUE *l = address<T>(last);
      SLOT_QUEUE_NEXT(l) = SLOT_QUEUE_SLOT(s);
      SLOT_QUEUE_PREV(s) = SLOT_QUEUE_SLOT(l);
      SLOT_QUEUE_PREV(&head) = SLOT_QUEUE_SLOT(s);
      SLOT_QUEUE_NEXT(s) = SLOT_QUEUE_SLOT(&head);
    }
  }

  void enqueue_front(T *t) {}

  void dequeue(T *t) {}

  template <typename F, typename... Args>
  void iterate(F &&f, Args &&... args) const noexcept {}

  template <typename F, typename... Args>
  void iterate_r(F &&f, Args &&... args) const noexcept {}

  size_t size() const noexcept {
    size_t count = 0;
    iterate([&count](T *p) -> bool {
      count++;
      return true;
    });
    return count;
  }

  template <typename F> T *find(F &&f) {
    T *p = nullptr;
    iterate([&p, f](T *x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F> T *find_r(F &&f) {
    T *p = nullptr;
    iterate_r([&p, f](T *x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F> SLOT_QUEUE *insert_before(T *p, F &&f) {
    T *x = find(f);
    if (x != nullptr) {
    }
    return x;
  }

  template <typename F> SLOT_QUEUE *insert_after(T *p, F &&f) {
    T *x = find(f);
    if (x != nullptr) {
    }
    return x;
  }
};
