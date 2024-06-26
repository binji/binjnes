#if defined(BINJNES_ATOMIC_C11)

#include <stdatomic.h>

typedef atomic_size_t AtomicSize;
typedef atomic_bool AtomicBool;

static inline size_t atomic_load_size(AtomicSize* addr) {
  return atomic_load(addr);
}

static inline void atomic_store_size(AtomicSize* addr, size_t value) {
  atomic_store(addr, value);
}

static inline bool atomic_load_bool(AtomicBool* addr) {
  return atomic_load(addr);
}

static inline void atomic_store_bool(AtomicBool* addr, bool value) {
  atomic_store(addr, value);
}

#elif defined(BINJNES_ATOMIC_MSVC)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef ERROR

typedef volatile PVOID AtomicSize;
typedef AtomicSize AtomicBool;

static inline size_t atomic_load_size(AtomicSize* addr) {
  return (size_t)*addr;
}

static inline void atomic_store_size(AtomicSize* addr, size_t value) {
  _InterlockedExchangePointer(addr, (PVOID)value);
}

static inline bool atomic_load_bool(AtomicBool* addr) {
  return (bool)atomic_load_size(addr);
}

static inline void atomic_store_bool(AtomicBool* addr, bool value) {
  atomic_store_size(addr, (size_t)value);
}

#else

#error "No atomics"

#endif
