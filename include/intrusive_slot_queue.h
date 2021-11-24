#pragma once
#include <stddef.h>
#include <stdint.h>

constexpr uint32_t npos = UINT32_MAX;

struct link {
  uint32_t next;
  uint32_t prev;

  link() : next(npos), prev(npos) {}
};

template <typename T, typename Alloc>
struct intrusive_slot_queue {
private:
  link head;

public:

  bool empty() const noexcept {
    return head.next == npos && head.prev == npos;
  }

  size_t size() const noexcept;

  T* front() const noexcept {
    return empty() ? nullptr : Alloc::address(head.next);
  }

  void enqueue_back(T& t);
  void enqueue_front(T& t);
};
