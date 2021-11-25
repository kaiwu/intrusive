#pragma once
#include <stddef.h>
#include <stdint.h>

constexpr uint32_t npos = UINT32_MAX;
typedef uint32_t SLOT_QUEUE[3];

#define SLOT_QUEUE_SLOT(q)       (*(uint32_t *) &((*(q))[0]))
#define SLOT_QUEUE_NEXT(q)       (*(uint32_t *) &((*(q))[1]))
#define SLOT_QUEUE_PREV(q)       (*(uint32_t *) &((*(q))[2]))

#define SLOT_QUEUE_DATA(ptr, type, field)                                          \
  ((type *) ((char *) (ptr) - offsetof(type, field)))

#define SLOT_QUEUE_ADDR(ptr, type, field)                                          \
  ((SLOT_QUEUE *) ((char *) (ptr) + offsetof(type, field)))

#define SLOT_QUEUE_INIT(q, s)                                                      \
  do {                                                                             \
    SLOT_QUEUE_SLOT(q) = (s);                                                      \
    SLOT_QUEUE_NEXT(q) = (s);                                                      \
    SLOT_QUEUE_PREV(q) = (s);                                                      \
  }                                                                                \
  while (0)

template<typename T>
T* address(uint32_t slot);

#define SLOT_QUEUE_EMPTY(q)                                                        \
  (SLOT_QUEUE_SLOT(q) == SLOT_QUEUE_NEXT(q))

template<typename T>
struct intrusive_slot_queue {
private:
  SLOT_QUEUE head;

public:
  intrusive_slot_queue() {
    SLOT_QUEUE_INIT(&head, npos);
  }

  bool empty() const noexcept {
  }

  T* front() const noexcept {
  }

  void enqueue_back(T* t) {
  }

  void enqueue_front(T* t) {
  }

  void dequeue(T* t) {
  }

  template <typename F, typename... Args>
  void iterate(F&& f, Args&&... args) const noexcept {
  }

  template <typename F, typename... Args>
  void iterate_r(F&& f, Args&&... args) const noexcept{
  }

  size_t size() const noexcept {
    size_t count = 0;
    iterate([&count](T* p) -> bool {
      count++;
      return true;
    });
    return count;
  }

  template <typename F>
  T* find(F&& f) {
    T* p = nullptr;
    iterate([&p, f](T* x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F>
  T* find_r(F&& f) {
    T* p = nullptr;
    iterate_r([&p, f](T* x) -> bool {
      if (f(x)) {
        p = x;
      }
      return p == nullptr;
    });
    return p;
  }

  template <typename F>
  T* insert_before(T* p, F&& f) {
    T* x = find(f);
    if (x != nullptr) {
    }
    return x;
  }

  template <typename F>
  T* insert_after(T* p, F&& f) {
    T* x = find(f);
    if (x != nullptr) {
    }
    return x;
  }
};
