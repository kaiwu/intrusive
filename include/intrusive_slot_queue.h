#pragma once
#include <stddef.h>
#include <stdint.h>

constexpr uint32_t npos = UINT32_MAX;

struct link {
  uint32_t next;
  uint32_t prev;

  link() : next(npos), prev(npos) {}
};

// Alloc::address(npos) -> nullptr
template <typename T, typename Alloc>
struct intrusive_slot_queue {
private:
  link head;

public:

  bool empty() const noexcept {
    return head.next == npos && head.prev == npos;
  }

  T* front() const noexcept {
    return empty() ? nullptr : Alloc::address(head.next);
  }

  void enqueue_back(T* t) {
    if (empty()) {
        head.next = t->slot;
        head.prev = t->slot;
        t->link.next = npos;
        t->link.prev = npos;
    }
    else {
        T* last = Alloc::address(head.prev);
        last->link.next = t->slot;
        t->link.prev = last->slot;
        t->link.next = npos;
        head.prev = t->slot;
    }
  }

  void enqueue_front(T* t) {
    if (empty()) {
        head.next = t->slot;
        head.prev = t->slot;
        t->link.next = npos;
        t->link.prev = npos;
    }
    else {
        T* first = Alloc::address(head.next);
        first->link.prev = t->slot;
        t->link.next = first->slot;
        t->link.prev = npos;
        head.next = t->slot;
    }
  }

  void dequeue(T* t) {
    T* p = Alloc::address(t->link.prev);
    T* n = Alloc::address(t->link.next);

    if (p == nullptr && n == nullptr) { // t is both head and tail
      head.next = npos; 
      head.prev = npos;
    }
    if (p == nullptr && n != nullptr) { // t is head
      head.next = n->slot; 
      n->link.prev = npos;
    }
    if (p != nullptr && n == nullptr) { // t is tail
      head.prev = p->slot; 
      p->link.prev = npos;
    }
    if (p != nullptr && n != nullptr) { // t is in the middle
      p->link.next = n->slot;
      n->link.prev = p->slot;
    }
    t->link = link();
  }

  template <typename F, typename... Args>
  void iterate(F&& f, Args&&... args) const noexcept {
    T* n = Alloc::address(head.next);
    while (n != nullptr) {
      if(!f(n, args...)) {
        break;
      }
      n = Alloc::address(n->link.next);
    }
  }

  template <typename F, typename... Args>
  void iterate_r(F&& f, Args&&... args) const noexcept{
    T* p = Alloc::address(head.prev);
    while (p != nullptr) {
      if(!f(p, args...)) {
        break;
      }
      p = Alloc::address(p->link.prev);
    }
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
};
