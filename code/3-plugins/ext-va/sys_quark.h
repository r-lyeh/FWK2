// -----------------------------------------------------------------------------
// ## string interning (quarks)
// - rlyeh, public domain.

API unsigned    intern( const char *string );
API const char *quark( unsigned key );

typedef struct quarks_db {
    array(char) blob;
    array(vec2i) entries;
} quarks_db;

API unsigned    quark_intern( quarks_db*, const char *string );
API const char *quark_string( quarks_db*, unsigned key );

// -----------------------------------------------------------------------------
// quarks

unsigned quark_intern( quarks_db *q, const char *string ) {
    if( string && string[0] ) {
        int slen = strlen(string);
        int qlen = array_count(q->blob);
        char *found;
        if( !qlen ) {
            array_resize(q->blob, slen + 1 );
            memcpy(found = q->blob, string, slen + 1);
        } else {
            found = strstr(q->blob, string);
            if( !found ) {
                array_resize(q->blob, qlen - 1 + slen + 1);
                memcpy(found = q->blob + qlen - 1, string, slen + 1 );
            }
        }
        // already interned? return that instead
        vec2i offset_len = vec2i(found - q->blob, slen);
        for( int i = 0; i < array_count(q->entries); ++i ) {
            if( offset_len.x == q->entries[i].x )
                if( offset_len.y == q->entries[i].y )
                    return i+1;
        }
        // else cache and return it
        array_push(q->entries, offset_len);
        return array_count(q->entries);
    }
    return 0;
}
const char *quark_string( quarks_db *q, unsigned key ) {
    if( key && key <= array_count(q->entries) ) {
        vec2i offset_len = q->entries[key-1];
        return va("%.*s", offset_len.y, q->blob + offset_len.x);
    }
    return "";
}

static __thread quarks_db qdb;
unsigned intern( const char *string ) {
    return quark_intern( &qdb, string );
}
const char *quark( unsigned key ) {
    return quark_string( &qdb, key );
}

#if 0
AUTORUN {
    test( !intern(NULL) ); // quark #0, cannot intern null string
    test( !intern("") );   // quark #0, ok to intern empty string
    test( !quark(0)[0] );  // empty string for quark #0

    unsigned q1 = intern("Hello");  // -> quark #1
    unsigned q2 = intern("happy");  // -> quark #2
    unsigned q3 = intern("world."); // -> quark #3
    printf("%u %u %u\n", q1, q2, q3);

    test( q1 );
    test( q2 );
    test( q3 );
    test( q1 != q2 );
    test( q1 != q3 );
    test( q2 != q3 );

    unsigned q4 = intern("happy");
    printf("%x vs %x\n", q2, q4);
    test( q4 );
    test( q4 == q2 );

    char buf[256];
    sprintf(buf, "%s %s %s", quark(q1), quark(q2), quark(q3));
    test( !strcmp("Hello happy world.", buf) );
}
#endif

