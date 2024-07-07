// 0,n  alloc
// p,n  realloc
// p,0  free
// p,~0 returns pointer size (beware: sizeof void* >= sizeof size_t)
// 0,~0 returns global mem stats (char*)

#include <stddef.h>
typedef void* (*allocator_t)(void*, size_t);

#if TEST == 1
#include <stdio.h>
#include <stdlib.h>
main() {
    allocator_t alloc = realloc;
    int *slot = alloc(NULL, sizeof(int));
    *slot = 42;
    printf("[%p] = %d\n", slot, *slot);
    slot = alloc(slot, 0);
    printf("[%p]\n", slot);
}
#endif

#if TEST == 2
#include <stdio.h>
#include <stdlib.h>
size_t arena_max;
void* arena(void *ptr, size_t n) {
    if(arena_max && n >= arena_max) return NULL;
    arena_max += (ptr ? _msize(ptr) : 0) - n;
    return realloc(ptr, n);
}
main() {
    arena_max = 7;
    allocator_t alloc = arena;
    int *x = alloc(0, sizeof(int)); printf("[%p]\n", x);
    int *y = alloc(0, sizeof(int)); printf("[%p]\n", y); alloc(x, 0);
    int *z = alloc(0, sizeof(int)); printf("[%p]\n", z);
}
#endif
