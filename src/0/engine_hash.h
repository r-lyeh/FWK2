// un/hash

API uint32_t unhash_32(uint32_t x);
API uint32_t hash_32(uint32_t x);
API uint64_t hash_64(uint64_t x);
API uint64_t hash_flt(double x);
API uint64_t hash_int(int key);
API uint64_t hash_ptr(const void* ptr);
API uint64_t hash_bin(const void* ptr, unsigned len);
API uint64_t hash_str(const char* str);

// -----------------------------------------------------------------------------
// hash helper utilities

API uint32_t hh_mem(const void *data, size_t size);
API uint32_t hh_str(const char* str);
API uint32_t hh_float(float f);
API uint32_t hh_int(int i);
API uint32_t hh_vec2(vec2 v);
API uint32_t hh_vec3(vec3 v);
API uint32_t hh_vec4(vec4 v);
API uint32_t hh_mat33(mat33 m);
API uint32_t hh_mat44(mat44 m);

