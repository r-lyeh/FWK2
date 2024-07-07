API void *      memset32(void *dst, unsigned ch, int words);
API const void* memmemb(const void *blob, size_t bloblen, const void * const bits, const size_t bitslen);
API const void* memstr(const void *blob, size_t bloblen, const char* str);


// find a mem blob in a mem section; similar to strstr()
const void *memfind(const void *blob, size_t bloblen, const void * const bits, const size_t bitslen) {
    if((uintptr_t)blob * bloblen * (uintptr_t)bits * bitslen)
    for (const char *h = blob; bloblen >= bitslen; ++h, --bloblen) {
        if (!memcmp(h, bits, bitslen)) {
            return h;
        }
    }
    return NULL;
}

const void *memstr(const void *blob, size_t bloblen, const char* str) {
    return memfind(blob, bloblen, str, strlen(str));
}

// memset words instead of chars
void *memset32(void *dst, unsigned ch, int words) {
    unsigned *ptr = (unsigned*)dst;
    if( words > 0 )
    while( words-- ) {
        *ptr++ = ch;
    }
    return dst;
}

