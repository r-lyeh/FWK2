// string utilities,
// - rlyeh, public domain

API char* va(const char *fmt, ...);

API int strmatch(const char *s, const char *wildcard);
API int strmatchi(const char *s, const char *wildcard);

API const char *strbegi(const char *a, const char *b); // returns initial match, if any. case insensitive
API const char *strendi(const char *src, const char *sub); // returns final match, if any. case insensitive
API const char* strstri(const char *a, const char *b);

#if CODE

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
char* va(const char *fmt, ...) {
    static char buf[64][1024+512];
    static int l = 0; l = (l+1) % 64;
    va_list vl;
    va_start(vl,fmt);
    int rc = vsnprintf(buf[l], 1024+512-1, fmt, vl);
    va_end(vl);
    buf[l][rc<0?0:rc] = 0;
    return buf[l];
}

#include <ctype.h>

int strmatch(const char *s, const char *wildcard) {
    // returns true if wildcard matches
    if( *wildcard == '\0' ) return !*s;
    if( *wildcard ==  '*' ) return strmatch(s, wildcard+1) || (*s && strmatch(s+1, wildcard));
    if( *wildcard ==  '?' ) return *s && (*s != '.') && strmatch(s+1, wildcard+1);
    return (*s == *wildcard) && strmatch(s+1, wildcard+1);
}
int strmatchi(const char *s, const char *wildcard) {
    // returns true if wildcard matches (case insensitive)
    if( *wildcard == '\0' ) return !*s;
    if( *wildcard ==  '*' ) return strmatchi(s, wildcard+1) || (*s && strmatchi(s+1, wildcard));
    if( *wildcard ==  '?' ) return *s && (*s != '.') && strmatchi(s+1, wildcard+1);
    return (tolower(*s) == tolower(*wildcard)) && strmatchi(s+1, wildcard+1);
}


const char *strstri( const char *src, const char *sub ){
    while( *src++ ) {
        for( const char *s = src-1, *f = sub, *c = s; ; ++f, ++c) {
            if(!*f) return s;
            if(!*c) return NULL;
            if(tolower(*c) != tolower(*f)) break;
        }
    }
    return NULL;
}
const char *strendi(const char *src, const char *sub) { // returns final match, if any. case insensitive
    int srclen = strlen(src);
    int sublen = strlen(sub);
    if( sublen > srclen ) return 0;
    return !strcmpi(src + srclen - sublen, sub) ? src + srclen - sublen : NULL;
}
const char *strbegi(const char *a, const char *b) { // returns initial match, if any. case insensitive
    int la = strlen(a), lb = strlen(b);
    if( la < lb ) return NULL;
    if( lb == 0 ) return a;
    int len = la < lb ? la : lb;
    for( int i = 0; i < len; ++i ) {
        if( tolower(a[i]) != tolower(b[i]) ) {
            return NULL;
        }
    }
    return a;
}

#endif
