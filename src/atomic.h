#if defined(BINJNES_GCC) || defined(BINJNES_CLANG)

#include <stdatomic.h>

typedef atomic_size_t AtomicSize;
static inline size_t atomic_load_size(AtomicSize* addr) {
    return atomic_load(addr);
}

static inline void atomic_store_size(AtomicSize* addr, size_t value) {
    atomic_store(addr, value);
}

#elif defined(BINJNES_MSVC)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef ERROR

typedef volatile PVOID AtomicSize;

size_t atomic_load_size(AtomicSize* addr) {
    return (size_t)*addr;
}

void atomic_store_size(AtomicSize* addr, size_t value) {
    _InterlockedExchangePointer(addr, (PVOID)value);
}

#endif