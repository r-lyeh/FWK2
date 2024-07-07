// - rlyeh, public domain

// -----------------------------------------------------------------------------
// less

API int less_64(uint64_t a, uint64_t b);
API int less_int(int a, int b);
API int less_ptr(void *a, void *b);
API int less_str(char *a, char *b);

// -----------------------------------------------------------------------------
// qsort

API int less_64_ptr(const void *a, const void *b);
API int less_int_ptr(const void *a, const void *b);
