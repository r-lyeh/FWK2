// -----------------------------------------------------------------------------
// less/sort

int less_64_ptr(const void *a, const void *b) {
    return 0[(uint64_t*)a] - 0[(uint64_t*)b];
}
int less_int_ptr(const void *a, const void *b) {
    return 0[(int*)a] - 0[(int*)b];
}

int less_int(int a, int b) {
    return a - b;
}
int less_64(uint64_t a, uint64_t b) {
    return a > b ? +1 : -!!(a - b);
}
int less_ptr(void *a, void *b) {
    return (uintptr_t)a > (uintptr_t)b ? +1 : -!!((uintptr_t)a - (uintptr_t)b);
}
int less_str(char *a, char *b) {
    return strcmp((const char *)a, (const char *)b);
}
