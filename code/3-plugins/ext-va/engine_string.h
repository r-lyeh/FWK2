// string framework
// - rlyeh, public domain

// string: temporary api (stack)
API char*   tempvl(const char *fmt, va_list);
API char*   tempva(const char *fmt, ...);
#define     va(...) (((&printf) || printf(__VA_ARGS__), tempva(__VA_ARGS__)))  // vs2015 check trick
#define     vac (const char*)va

// string: allocated api (heap). FREE() after use
API char*   strcatf(char **s, const char *buf);
#define     strcatf(s,fmt,...)  strcatf((s), va(fmt, __VA_ARGS__))
#define     stringf(fmt,...)    STRDUP(va(fmt, __VA_ARGS__)) // (strcatf)(0, va(fmt, __VA_ARGS__))


#if is(cl) || (is(tcc) && is(win32))
#if!is(cl)
char* strtok_s(char* str,const char* delimiters,char** context); // tcc misses this in <string.h>
#endif
#define strtok_r strtok_s
#endif

#if 1
#define each_substring(str, delims, keyname) \
    ( char *str_ = (char*)(str); str_; str_ = 0 ) \
    for( int len_ = strlen(str_) + 1, heap_ = len_ < 1024; len_ > 1; len_ = 0 ) \
    for( char *ptr_ = (heap_ ? REALLOC(0, len_) : ALLOCA(len_)), *cpy_ = (snprintf(ptr_, len_, "%s", str_), ptr_); ptr_; (heap_ ? REALLOC(ptr_, 0) : 0), ptr_ = 0 ) \
    for( char *next_token = 0, *keyname = strtok_r(cpy_, delims, &next_token); keyname; keyname = strtok_r(NULL, delims, &next_token) )
#else
#define each_substring(str, delims, keyname) \
    ( char** tokens_ = strsplit((str), (delims)), *keyname = 0; tokens_; tokens_ = 0) \
    for( int i_ = 0, end_ = array_count(tokens_); i_ < (keyname = tokens_[i_], end_); ++i_ )
#endif

// utils

API int          strcmp_qsort(const void *a, const void *b);
API int          strcmpi_qsort(const void *a, const void *b);

API bool         strbeg(const char *src, const char *sub); // returns true if both strings match at beginning. case sensitive
API bool         strend(const char *src, const char *sub); // returns true if both strings match at end. case sensitive

API bool         strbegi(const char *src, const char *sub);  // returns true if both strings match at beginning. case insensitive
API bool         strendi(const char *src, const char *sub);  // returns true if both strings match at end. case insensitive
API const char * strstri(const char *src, const char *sub);  // returns find first substring in string. case insensitive.
#define          strcmpi  ifdef(cl, _stricmp, strcasecmp)

API char *       strupper(const char *str);
API char *       strlower(const char *str);

API char *       strrepl(char **copy, const char *target, const char *replace); // replace any 'target' as 'repl' in 'copy'. 'copy' may change (heap). returns 'copy'
API char *       strswap(char *copy, const char *target, const char *replace);  // replaced inline only if repl is shorter than target. no allocations.
API char *       strcut(char *copy, const char *target);                        // remove any 'target' in 'copy'. returns 'copy'

API const char * strlerp(unsigned numpairs, const char **pairs, const char *str); // using key-value pairs, null-terminated

#ifndef __APPLE__ // BSD provides these
API size_t       strlcat(char *dst, const char *src, size_t dstcap); // concat 2 strings safely. always NUL terminates. may truncate.
API size_t       strlcpy(char *dst, const char *src, size_t dstcap); // copy 2 strings safely. always NUL terminates. truncates if retval>=dstcap
#endif

/// split `string` after any of `delimiters` character is found.
/// returns temporary array of split strings. see: strjoin
/// > array(char*) tokens = strsplit("hello! world!", " !"); // [0]="hello",[1]="world",
API array(char*)    strsplit(const char *string, const char *delimiters);

/// concatenate all elements within `list`, with `separator` string in between.
/// returns: temporary joint string. see: strsplit
/// > array(char*) tokens = strsplit("hello! world!", " !"); // [0]="hello",[1]="world",
/// > char *joint = strjoin(tokens, "+"); // joint="hello+world"
API char*           strjoin(array(char*) list, const char *separator);










#include <stdarg.h>

#ifndef STACK_ALLOC_SIZE
#define STACK_ALLOC_SIZE (2048*1024)
#endif

char* tempvl(const char *fmt, va_list vl) {
    va_list copy;
    va_copy(copy, vl);
    int sz = /*stbsp_*/vsnprintf( 0, 0, fmt, copy ) + 1;
    va_end(copy);

    int reqlen = sz;
#if 0
    int heap = 0;
    enum { STACK_ALLOC = 16384 };
    static __thread char buf[STACK_ALLOC];
#else
    int heap = 1;
    static __thread int STACK_ALLOC = STACK_ALLOC_SIZE;
    static __thread char *buf = 0; if(!buf) buf = REALLOC(0, STACK_ALLOC); // @leak
#endif
    static __thread int cur = 0; //printf("string stack %d/%d\n", cur, STACK_ALLOC);

    if( reqlen >= STACK_ALLOC ) {
        tty_color(RED);
        printf("no stack enough, increase STACK_ALLOC variable above (reqlen:%d) (fmt: %s)\n", reqlen, fmt);
        tty_color(0);
        //assert(reqlen < STACK_ALLOC);
        STACK_ALLOC = reqlen * 2;
        buf = REALLOC(0, STACK_ALLOC);
    }

    char* ptr = buf + (cur *= (cur+reqlen) < (STACK_ALLOC - 1), (cur += reqlen) - reqlen);

    /*stbsp_*/vsnprintf( ptr, sz, fmt, vl );
    return (char *)ptr;
}
char* tempva(const char *fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    char *s = tempvl(fmt, vl);
    va_end(vl);
    return s;
}

char* (strcatf)(char **src_, const char *buf) {
    char *src = *src_;
        if(!buf) return src;
        // if(!buf[0]) return src;
        int srclen = (src ? strlen(src) : 0), buflen = strlen(buf);
        src = (char*)REALLOC(src, srclen + buflen + 1 );
        memcpy(src + srclen, buf, buflen + 1 );
    *src_ = src;
    return src;
}

// -----------------------------------------------------------------------------
// string utils

int strcmp_qsort(const void *a, const void *b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}
int strcmpi_qsort(const void *a, const void *b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmpi(*ia, *ib);
}

bool strbeg(const char *a, const char *b) { // returns true if both strings match at beginning. case sensitive
    return strncmp(a, b, strlen(b)) ? false : true; // strstr(a,b) == a
}
bool strend(const char *a, const char *b) { // returns true if both strings match at end. case sensitive
    int la = strlen(a), lb = strlen(b);
    if( la < lb ) return false;
    return strncmp(a + la - lb, b, lb) ? false : true;
}
/*
int main() {
    printf("strbeg(abc abc) = %d\n", strbeg("abc", "abc"));
    printf("strbeg(abc abcd) = %d\n", strbeg("abc", "abcd"));
    printf("strbeg(abcd abc) = %d\n", strbeg("abcd", "abc"));
    printf("strbeg(abc (empty)) = %d\n", strbeg("abc", ""));
    printf("strbeg((empty) abc) = %d\n", strbeg("", "abc"));
    printf("strbeg(123 abcd) = %d\n", strbeg("123", "abcd"));
    printf("strbeg(abcd 123) = %d\n", strbeg("abcd", "123"));
    puts("---");
    printf("strend(abc abc) = %d\n", strend("abc", "abc"));
    printf("strend(abc 0abc) = %d\n", strend("abc", "0abc"));
    printf("strend(abcd bcd) = %d\n", strend("abcd", "bcd"));
    printf("strend(abc (empty)) = %d\n", strend("abc", ""));
    printf("strend((empty) abc) = %d\n", strend("", "abc"));
    printf("strend(123 abcd) = %d\n", strend("123", "abcd"));
    printf("strend(abcd 123) = %d\n", strend("abcd", "123"));
}
*/

bool strendi(const char *src, const char *sub) { // returns true if both strings match at end. case insensitive
    int srclen = strlen(src);
    int sublen = strlen(sub);
    if( sublen > srclen ) return 0;
    return !strcmpi(src + srclen - sublen, sub);
}


// Find substring in string, case insensitive. Alias for strcasestr()
// Returns first char of coincidence, or NULL.
const char *strstri(const char *a, const char *b) {
    char *A, *B, *M;
    for(char *p = (char*)(A = va("%s",a)); *p; ++p) *p = toupper(*p);
    for(char *p = (char*)(B = va("%s",b)); *p; ++p) *p = toupper(*p);
    return M = strstr(A, B), M ? a + (M - A) : NULL;
}

char *strupper(const char *str) {
    char *s = va("%s", str), *bak = s;
    while(*s++) s[-1] = toupper(s[-1]);
    return bak;
}
char *strlower(const char *str) {
    char *s = va("%s", str), *bak = s;
    while(*s++) s[-1] = tolower(s[-1]);
    return bak;
}

#ifndef __APPLE__ // BSD provides these
// Safely concatenate two strings. Always NUL terminates (unless dstcap == 0).
// Returns length of operation; if retval >= dstcap, truncation occurred.
size_t strlcat(char *dst, const char *src, size_t dstcap) {
    int dl = strlen(dst), sl = strlen(src);
    if( dstcap ) snprintf(dst + dl, dstcap - dl, "%s", src);
    return dl + sl;
}
// Safely copy two strings. Always NUL terminates (unless dstcap == 0).
// Copy src to string dst of size dstcap. Copies at most dstcap-1 characters.
// Returns length of input; if retval >= dstcap, truncation occurred.
size_t strlcpy(char *dst, const char *src, size_t dstcap) {
    int sl = strlen(src);
    if( dstcap ) snprintf(dst, dstcap, "%*s", sl, src);
    return sl;// count does not include NUL
}
#endif

char *string8(const wchar_t *str) { // from wchar16(win) to utf8/ascii
    int i = 0, n = wcslen(str) * 6 - 1;
    static __thread char error[1] = {0}, buffer[2048]; assert( n < 2048 );
    while( *str ) {
       if (*str < 0x80) {
          if (i+1 > n) return error;
          buffer[i++] = (char) *str++;
       } else if (*str < 0x800) {
          if (i+2 > n) return error;
          buffer[i++] = 0xc0 + (*str >> 6);
          buffer[i++] = 0x80 + (*str & 0x3f);
          str += 1;
       } else if (*str >= 0xd800 && *str < 0xdc00) {
          uint32_t c;
          if (i+4 > n) return error;
          c = ((str[0] - 0xd800) << 10) + ((str[1]) - 0xdc00) + 0x10000;
          buffer[i++] = 0xf0 + (c >> 18);
          buffer[i++] = 0x80 + ((c >> 12) & 0x3f);
          buffer[i++] = 0x80 + ((c >>  6) & 0x3f);
          buffer[i++] = 0x80 + ((c      ) & 0x3f);
          str += 2;
       } else if (*str >= 0xdc00 && *str < 0xe000) {
          return error;
       } else {
          if (i+3 > n) return error;
          buffer[i++] = 0xe0 + (*str >> 12);
          buffer[i++] = 0x80 + ((*str >> 6) & 0x3f);
          buffer[i++] = 0x80 + ((*str     ) & 0x3f);
          str += 1;
       }
    }
    buffer[i] = 0;
    return va("%s", buffer);
}

char *strrepl(char **string, const char *target, const char *replace) { // may reallocate input string if needed
    //if new text is shorter than old one,then no need to heap, replace inline
    int rlen = strlen(replace), tlen = strlen(target), diff = tlen - rlen;
    if( diff >= 0 ) return strswap(*string, target, replace);

    char *buf = 0, *aux = *string;
    for( int tgtlen = tlen; tgtlen && aux[0]; ) {
        char *found = strstr(aux, target);
        if( found ) {
            strcatf(&buf, "%.*s%s", (int)(found - aux), aux, replace);
            aux += (found - aux) + tgtlen;
        } else {
            strcatf(&buf, "%s", aux);
            break;
        }
    }
    if( buf ) {
        // strcpy(*string, buf);
        char *s = *string;
        if(s) s[0] = 0;
        strcatf(&s, "%s", buf);
        *string = s;

        FREE( buf );
    }
    return *string;
}
char *strswap(char *copy, const char *target, const char *replace) { // replaced only if new text is shorter than old one
    int rlen = strlen(replace), diff = strlen(target) - rlen;
    if( diff >= 0 ) {
        for( char *s = copy, *e = s + strlen(copy); /*s < e &&*/ 0 != (s = strstr(s, target)); ) {
            if( rlen ) s = (char*)memcpy( s, replace, rlen ) + rlen;
            if( diff ) memmove( s, s + diff, (e - (s + diff)) + 1 );
        }
    }
    return copy;
}
char *strcut(char *copy, const char *target) {
    return strswap(copy, target, "");
}

const char *strlerp(unsigned numpairs, const char **pairs, const char *str) { // using key-value pairs, null-terminated
    if( !pairs[0] ) {
        return str;
    }
    // find & replace all tokens; @fixme: optimize me
    char *buf = REALLOC(0, 128*1024); strcpy(buf, str);
    for( unsigned i = 0; i < numpairs; ++i ) {
        const char *token = pairs[i*2+0];
        const char *repl = pairs[i*2+1];
        while(strstr(buf, token)) {
            strrepl(&buf, token, repl);
        }
    }
    char *ret = va("%s", buf);
    FREE(buf);
    return ret;
}

array(char*) strsplit(const char *str, const char *separators) {
    enum { SLOTS = 32 };
    static __thread int slot = 0;
    static __thread char *buf[SLOTS] = {0};
    static __thread array(char*) list[SLOTS] = {0};

    slot = (slot+1) % SLOTS;
    array_resize(list[slot], 0);
    *(buf[slot] = REALLOC(buf[slot], strlen(str)*2+1)) = '\0'; // *2 to backup pathological case where input str is only separators && include == 1

    for(char *dst = buf[slot]; str && *str; ) {
        // count literal run && terminators
        int run = strcspn(str, separators);
        int end = strspn(str + run, separators);

        // append literal run
        if( run ) {
            array_push(list[slot], dst);
            memmove(dst,str,run); dst[run] = '\0'; //strncpy(dst, str, run)
            dst += run + 1;
        }
#if defined SPLIT_INCLUDE_SEPARATORS && SPLIT_INCLUDE_SEPARATORS
        // mode: append all separators: "1++2" -> "1" "+" "+" "2"
        for( int i = 0; i < end; ++i ) {
            array_push(list[slot], dst);
            dst[0] = str[ run + i ];
            dst[1] = '\0';
            dst += 2;
        }
#endif

        // skip both
        str += run + end;
    }

    return list[slot];
}
char* strjoin(array(char*) list, const char *separator) {
    enum { SLOTS = 16 };
    static __thread int slot = 0;
    static __thread char* mems[SLOTS] = {0};

    slot = (slot+1) % SLOTS;

    int num_list = array_count(list);
    int len = 0, inc = 0, seplen = strlen(separator);
    for( int i = 0; (num_list > 0 ? i < num_list : !!list[i]); ++i ) {
        len += strlen(list[i]) + inc;
        inc = seplen;
    }

    mems[slot] = REALLOC(mems[slot], len+1);
    char *p = mems[slot]; *p = 0;
    const char *sep = "";
    for( int i = 0; (num_list > 0 ? i < num_list : !!list[i]); ++i ) {
        p += sprintf(p, "%s%s", sep, list[i]);
        sep = separator;
    }

    return mems[slot];
}
