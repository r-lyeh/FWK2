// string utilities,
// - rlyeh, public domain

API char** split(char *buffer, const char *separators);
API char* strsep(char **sp, const char *sep);

API char* replace( char *copy, const char *target, const char *replacement );


#if CODE

#include <string.h>
char *replace( char *copy, const char *target, const char *replacement ) {
    // replaced only if new text is shorter than old one
    int rlen = strlen(replacement), diff = strlen(target) - rlen;
    if( diff < 0 ) return 0;
    for( char *s = copy, *e = s + strlen(copy); /*s < e &&*/ 0 != (s = strstr(s, target)); ) {
        if( rlen ) s = (char*)memcpy( s, replacement, rlen ) + rlen;
        if( diff ) memmove( s, s + diff, (e - (s + diff)) + 1 );
    }
    return copy;
}
int qsort_strcmp(const void * a, const void * b ) {
    // smart strcmp which does:
    // Narc - 128k.tzx < Narc - 128k - Levels.tzx, and also:
    // Academy - Side 1.tzx < Academy - Side 2.tzx < Academy - Side 3.tzx < Academy - Side 4.tzx, and also:
    // Outrun - Tape 1 - Side 1.tzx < Outrun - Tape 2 - Side 1.tzx < Outrun - Tape 2 - Side 2.tzx
    const char *pa = *(const char**)a;
    const char *pb = *(const char**)b;
    int la = strlen(pa), lb = strlen(pb);
    if( la < lb ) return -1;
    if( la > lb ) return +1;
    return strcmp(pa,pb);
}

#ifdef _WIN32 // better than strtok(). preserves empty strings within delimiters
char *strsep(char **sp, const char *sep) {
    if( sp && *sp && **sp ) {
        char *p = *sp + strcspn(*sp, sep), *s = *sp;
        if( *p ) *p++ = '\0';
        *sp = p;
        return s;
    }
    return NULL;
}
#endif

#if 0
char** split(char *buffer, const char *separators) {
    static int slot = 0; slot = (slot + 1) % 16;
    static char **tokens[16] = {0}; // @fixme: slots
    tokens[slot] = realloc(tokens[slot], strlen(buffer)/2 + 1);
    char **token = tokens[slot];
    for( char *sep = buffer, *ptr = strsep(&sep, separators); ptr; ptr = strsep(&sep, separators) ) {
        *token++ = ptr;
    }
    *token++ = 0;
    return tokens[slot];
}
#endif

#endif // CODE

