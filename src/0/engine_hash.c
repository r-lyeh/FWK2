// -----------------------------------------------------------------------------
// un/hash

uint32_t unhash_32(uint32_t x) {
    // Thomas Mueller at https://stackoverflow.com/questions/664014/ - says no collisions for 32bits!
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = ((x >> 16) ^ x) * 0x119de1f3;
    x = (x >> 16) ^ x;
    return x;
}
uint32_t hash_32(uint32_t x) {
    // Thomas Mueller at https://stackoverflow.com/questions/664014/ - says no collisions for 32bits!
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
uint64_t hash_64(uint64_t x) {
#if 1
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
#else
    // should we just use x2 hash_32?
    uint32_t hi = (x >> 32ull), lo = (x & ~0u);
    return (hash_32(hi) << 32ull) | hash_32(lo);
#endif
}
uint64_t hash_flt(double x) {
    union { double d; uint64_t i; } c;
    return c.d = x, hash_64(c.i);
}
uint64_t hash_str(const char* str) {
    uint64_t hash = 14695981039346656037ULL; // hash(0),mul(131) faster than fnv1a, a few more collisions though
    while( *str ) hash = ( (unsigned char)*str++ ^ hash ) * 0x100000001b3ULL;
    return hash;
}
uint64_t hash_bin(const void* ptr, unsigned len) {
    uint64_t hash = 14695981039346656037ULL; // hash(0),mul(131) faster than fnv1a, a few more collisions though
    for( unsigned char *str = (unsigned char *)ptr; len--; )
    hash = ( (unsigned char)*str++ ^ hash ) * 0x100000001b3ULL;
    return hash;
}
uint64_t hash_int(int key) {
    return hash_32((uint32_t)key);
}
uint64_t hash_ptr(const void *ptr) {
    uint64_t key = (uint64_t)(uintptr_t)ptr;
    return hash_64(key); // >> 3? needed?
}

// hash helper utilities

uint32_t hh_mem(const void *data, size_t size) {
    const uint8_t *ptr = (const uint8_t *)data;
    uint32_t hash = 0;
    for (size_t i = 0; i < size; ++i) {
        hash = (hash * 31) + ptr[i];
    }
    return hash;
}

uint32_t hh_str(const char* str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

uint32_t hh_float(float f) {
    union { float f; uint32_t i; } u;
    u.f = f;
    return u.i;
}

uint32_t hh_int(int i) {
    return (uint32_t)i;
}

uint32_t hh_vec2(vec2 v) {
    return hh_float(v.x) ^ (hh_float(v.y) << 1);
}

uint32_t hh_vec3(vec3 v) {
    return hh_float(v.x) ^ (hh_float(v.y) << 1) ^ (hh_float(v.z) << 2);
}

uint32_t hh_vec4(vec4 v) {
    return hh_float(v.x) ^ (hh_float(v.y) << 1) ^ (hh_float(v.z) << 2) ^ (hh_float(v.w) << 3);
}

uint32_t hh_mat33(mat33 m) {
    uint32_t hash = 0;
    for (int i = 0; i < 9; ++i)
        hash ^= hh_float(m[i]) << i;
    return hash;
}

uint32_t hh_mat44(mat44 m) {
    uint32_t hash = 0;
    for (int i = 0; i < 16; ++i)
        hash ^= hh_float(m[i]) << i;
    return hash;
}
