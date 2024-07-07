// -----------------------------------------------------------------------------
// file

#if 0 // ifdef _WIN32
#include <winsock2.h>
#if is(tcc)
    #define CP_UTF8 65001
    int WINAPI MultiByteToWideChar();
    int WINAPI WideCharToMultiByte();
#endif
// widen() ? string1252() ? string16() ? stringw() ?
wchar_t *widen(const char *utf8) { // wide strings (win only)
    int chars = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    char *buf = va("%.*s", (int)(chars * sizeof(wchar_t)), "");
    return MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (void*)buf, chars), (wchar_t *)buf;
}
#define open8(path,mode)  ifdef(win, _wopen(widen(path))               ,  open(path, mode) )
#define fopen8(path,mode) ifdef(win, _wfopen(widen(path),widen(mode))  ,  fopen(path,mode) )
#define remove8(path)     ifdef(win, _wremove(widen(path))             ,  remove(path)     )
#define rename8(path)     ifdef(win, _wrename(widen(path))             ,  rename(path)     )
#define stat8(path,st)    ifdef(win, _wstat(widen(path),st)            ,  stat(path,st)    ) // _stati64()
#define stat8_t           ifdef(win, _stat                             ,  stat_t           ) // struct _stati64
#endif


static inline
char *file_preprocess(const char *src, const char *path, char* (*fs_read)(const char *fname), const char *parent_function) { // must FREE() after use
    if (!src) return NULL;

    char *includes = NULL;
    for each_substring(src, "\n", line) {
        if (line[0] == '#' && strstri(line, "#include")) {
            const char *start = strstri(line, "\"");
            const char *end = strstri(start+1, "\"");
            if (start && end) {
                char *filename = va("%s%.*s", path ? path : "", (int)(end-start-1), start+1);
                char *included = fs_read(filename);
                if (included) {
                    char *nested_includes = file_preprocess(included, path, fs_read, parent_function);
                    includes = strcatf(&includes, "%s\n", nested_includes ? nested_includes : ""); //@leak
                } else {
                    PANIC("!ERROR: %s: Include file not found: %s\n", parent_function, filename);
                }
            } else {
                PANIC("!ERROR: %s: Invalid #include directive: %s\n", parent_function, line);
            }
        } else 
        {
            includes = strcatf(&includes, "\n%s", line); //@leak
        }
    }

    return includes;
}


char *file_name(const char *pathfile) {
    char *s = strrchr(pathfile, '/'), *t = strrchr(pathfile, '\\');
    return va("%s", s > t ? s+1 : t ? t+1 : pathfile);
}
char *file_base(const char *pathfile) {
    char *s = file_name(pathfile);
    char *e = file_ext(pathfile);
    return s[ strlen(s) - strlen(e) ] = '\0', s;
}
char *file_pathabs( const char *pathfile ) {
    char *out = va("%*.s", DIR_MAX+1, "");
#if is(win32)
    _fullpath(out, pathfile, DIR_MAX);
#else
    realpath(pathfile, out);
#endif
    return out;
}
char *file_path(const char *pathfile) {
    return va("%.*s", (int)(strlen(pathfile)-strlen(file_name(pathfile))), pathfile);
}
char *file_load(const char *filename, int *len) { // @todo: 32 counters/thread enough?
    static __thread array(char) buffer[32] = {0}, *invalid[1];
    static __thread unsigned i = 0; i = (i+1) % 32;

    FILE *fp = filename[0] ? fopen(filename, "rb") : NULL;
    if( fp ) {
        fseek(fp, 0L, SEEK_END);
        size_t sz = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        array_resize(buffer[i], sz+1);
        sz *= fread(buffer[i],sz,1,fp) == 1;
        buffer[i][sz] = 0;
        if(len) *len = (int)sz;
        fclose(fp);
        return buffer[i]; // @fixme: return 0 on error instead?
    }
    if (len) *len = 0;
    return 0;
}
char *file_read(const char *filename) { // @todo: fix leaks
    return file_load(filename, NULL);
}
bool file_write(const char *name, const void *ptr, int len) {
    bool ok = 0;
    for( FILE *fp = name && ptr && len >= 0 ? fopen(name, "wb") : NULL; fp; fclose(fp), fp = 0) {
        ok = fwrite(ptr, len,1, fp) == 1;
    }
    return ok;
}
bool file_append(const char *name, const void *ptr, int len) {
    bool ok = 0;
    for( FILE *fp = name && ptr && len >= 0 ? fopen(name, "a+b") : NULL; fp; fclose(fp), fp = 0) {
        ok = fwrite(ptr, len,1, fp) == 1;
    }
    return ok;
}
static // not exposed
bool file_stat(const char *fname, struct stat *st) {
    // remove ending slashes. win32+tcc does not like them.
    int l = strlen(fname), m = l;
    while( l && (fname[l-1] == '/' || fname[l-1] == '\\') ) --l;
    fname = l == m ? fname : va("%.*s", l, fname);
    return stat(fname, st) >= 0;
}
uint64_t file_stamp(const char *fname) {
    struct stat st;
    return !file_stat(fname, &st) ? 0ULL : st.st_mtime;
}
uint64_t file_stamp10(const char *fname) {
    time_t mtime = (time_t)file_stamp(fname);
    struct tm *ti = localtime(&mtime);
    return atoi64(va("%04d%02d%02d%02d%02d%02d",ti->tm_year+1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec));
}
uint64_t file_size(const char *fname) {
    struct stat st;
    return !file_stat(fname, &st) ? 0ULL : st.st_size;
}
bool file_directory( const char *pathfile ) {
    struct stat st;
    return !file_stat(pathfile, &st) ? 0 : S_IFDIR == ( st.st_mode & S_IFMT );
}
bool file_exist(const char *fname) {
    struct stat st;
    return !file_stat(fname, &st) ? false : true;
}
char *file_normalize(const char *name) {
    char *copy = va("%s", name), *s = copy, c;
#if is(win32)
    for( int i = 0; copy[i]; ++i ) { if(copy[i] == '/') copy[i] = '\\'; if(copy[i] == '\'') copy[i] = '\"'; }
#else
    for( int i = 0; copy[i]; ++i ) { if(copy[i] == '\\') copy[i] = '/'; if(copy[i] == '\"') copy[i] = '\''; }
#endif
    return copy;
}
#if 0
char *file_normalize(const char *name) {
    char *copy = va("%s", name), *s = copy, c;
    // lowercases+digits+underscores+slashes only. anything else is truncated.
    for( ; *name ; ++name ) {
        /**/ if( *name >= 'a' && *name <= 'z' ) *s++ = *name;
        else if( *name >= 'A' && *name <= 'Z' ) *s++ = *name - 'A' + 'a';
        else if( *name >= '0' && *name <= '9' ) *s++ = *name;
        else if( *name == '/' || *name == '\\') *s++ = '/';
        else if( *name <= ' ' || *name == '.' ) *s++ = '_';
    } *s++ = 0;
    // remove dupe slashes
    for( name = s = copy, c = '/'; *name ; ) {
        while( *name && *name != c ) *s++ = *name++;
        if( *name ) *s++ = c;
        while( *name && *name == c ) name++;
    } *s++ = 0;
    // remove dupe underlines
    for( name = s = copy, c = '_'; *name ; ) {
        while( *name && *name != c ) *s++ = *name++;
        if( *name ) *s++ = c;
        while( *name && *name == c ) name++;
    } *s++ = 0;
    return copy;
}
char *file_normalize_with_folder(const char *name) {
    char *s = file_normalize(name);
    char *slash = strrchr(s, '/'); if(slash) *slash = 0;
    char *penultimate = strrchr(s, '/'); if(slash) *slash = '/';
    return penultimate ? penultimate+1 : /*slash ? slash+1 :*/ s;
}
#endif
char *file_ext(const char *name) {
    char *b = file_name(name), *s = strchr(b, '.'); //strrchr(name, '.');
    return va("%s", s ? s : "" ); // , name );
}

char *file_id(const char *pathfile) {
    char *dir = file_path(pathfile); for(int i=0;dir[i];++i) dir[i]=tolower(dir[i]);
    char *base = file_name(pathfile); for(int i=0;base[i];++i) base[i]=tolower(base[i]);
#if 0 // extensionless, larry.mid and larry.txt will collide, diffuse.tga and diffuse.png will match
char *ext = strchr(base, '.'); if (ext) ext[0] = '\0'; // remove all extensions
#else // extensionless for audio/images only (materials: diffuse.tga and diffuse.png will match)
char *ext = strrchr(base, '.'); //if (ext) ext[0] = '\0'; // remove all extensions
    if(ext) if( strstr(".jpg.png.bmp.tga.hdr"".", ext) || strstr(".ogg.mp3.wav.mod.xm.flac"".", ext) || strstr(".mp4.ogv.avi.mkv.wmv.mpg.mpeg"".", ext) ) {
        ext = strchr(base, '.');
        ext[0] = '\0'; //strcpy(ext, "_xxx");
    }
#endif

// if (!dir[0]) return base;
    char *stem = va("%s/%s", dir, base); // file_name(dir);

    // /path2/path1/file2_file1 -> file1_file2/path1/path2
    int len = 0;
    int ids_count = 0;
    char ids[64][64] = { 0 };
    // split path stems
    for each_substring(stem, "/\\@", key) {
        int tokens_count = 0;
        char* tokens[64] = { 0 };
        // split tokens
        for each_substring(key, "[]()_ ", it) {
            tokens[tokens_count++] = va("%s", it);
        }
        // sort alphabetically
        if( tokens_count > 1 ) qsort(tokens, tokens_count, sizeof(char *), strcmp_qsort);
        // concat sorted token1_token2_...
        char built[256]; *built = 0;
        for( int i = 0; i < tokens_count; ++i ) {
            strlcat(built, "_", 256);
            strlcat(built, tokens[i], 256);
        }
        strncpy( ids[ ids_count ], &built[1], 64 );
        len += strlen( ids[ ids_count++ ] );
    }
    // concat in inverse order: file/path1/path2/...
    char buffer[DIR_MAX]; buffer[0] = 0;
    for( int it = ids_count; --it >= 0; ) {
        strcat(buffer, ids[it]);
        strcat(buffer, "/");
    }
    return va("%s", buffer);
}
array(char*) file_list(const char *pathmasks) {
    static __thread array(char*) list = 0; // @fixme: add 16 slots

    for( int i = 0; i < array_count(list); ++i ) {
        FREE(list[i]);
    }
    array_resize(list, 0);

    for each_substring(pathmasks,";",pathmask) {
        char *cwd = 0, *masks = 0;
            char *slash = strrchr(pathmask, '/');
            if( !slash ) cwd = "./", masks = pathmask;
            else {
                masks = va("%s", slash+1);
                cwd = pathmask, slash[1] = '\0';
            }
            if( !masks[0] ) masks = "*";

        ASSERT(strend(cwd, "/"), "Error: dirs like '%s' must end with slash", cwd);

        int recurse = strstr(cwd, "**") || strstr(masks, "**");
        strswap(cwd, "**", "./");

        dir *d = dir_open(cwd, recurse ? "r" : "");
        if( d ) {
            for( int i = 0; i < dir_count(d); ++i ) {
                if( dir_file(d,i) ) {
                    // dir_name() should return full normalized paths "C:/project/demos/art/fx/fxBloom.fs". should exclude system dirs as well
                    char *entry = dir_name(d,i);
                    char *fname = file_name(entry);

                    int allowed = 0;
                    for each_substring(masks,";",mask) {
                        allowed |= strmatch(fname, mask);
                    }
                    if( !allowed ) continue;

                    // if( strstr(fname, "/.") ) continue; // @fixme: still needed? useful?

                    // insert copy
                    char *copy = STRDUP(entry);
                    array_push(list, copy);
                }
            }
            dir_close(d);
        }
    }

    array_sort(list, strcmp);
    return list;
}

bool file_move(const char *src, const char *dst) {
    bool ok = file_exist(src) && !file_exist(dst) && 0 == rename(src, dst);
    return ok;
}
bool file_delete(const char *pathfile) {
    if( file_exist(pathfile) ) {
        for( int i = 0; i < 10; ++i ) {
            bool ok = 0 == unlink(pathfile);
            if( ok ) return true;
            sleep_ms(10);
        }
        return false;
    }
    return true;
}
bool file_copy(const char *src, const char *dst) {
    int ok = 0, BUFSIZE = 1 << 20; // 1 MiB
    static __thread char *buffer = 0; do_once buffer = REALLOC(0, BUFSIZE); // @leak
    for( FILE *in = fopen(src, "rb"); in; fclose(in), in = 0) {
        for( FILE *out = fopen(dst, "wb"); out; fclose(out), out = 0, ok = 1) {
            for( int n; !!(n = fread( buffer, 1, BUFSIZE, in )); ){
                if(fwrite( buffer, 1, n, out ) != n)
                return fclose(in), fclose(out), false;
            }
        }
    }
    return ok;
}

char* file_tempname() {
    static __thread int id;
    return va("%s/framework-temp.%s.%p.%d", app_temp(), getenv(ifdef(win32, "username", "USER")), &id, rand());
}
FILE *file_temp(void) {
    const char *fname = file_tempname();
    FILE *fp = fopen(fname, "w+b");
    if( fp ) unlink(fname);
    return fp;
}

char *file_counter(const char *name) {
    static __thread char outfile[DIR_MAX], init = 0;
    static __thread map(char*, int) ext_counters;
    if(!init) map_init(ext_counters, less_str, hash_str), init = '\1';

    char *base = va("%s",name), *ext = file_ext(name);
    if(ext && ext[0]) *strstr(base, ext) = '\0';

    int *counter = map_find_or_add(ext_counters, ext, 0);
    while( *counter >= 0 ) {
        *counter = *counter + 1;
        sprintf(outfile, "%s(%03d)%s", base, *counter, ext);
        if( !file_exist(outfile) ) {
            return va("%s", outfile);
        }
    }
    return 0;
}

enum { MD5_HASHLEN = 16 };
enum { SHA1_HASHLEN = 20 };
enum { CRC32_HASHLEN = 4 };

void* file_sha1(const char *file) { // 20bytes
    hash_state hs = {0};
    sha1_init(&hs);
    for( FILE *fp = fopen(file, "rb"); fp; fclose(fp), fp = 0) {
        char buf[8192];
        for( int inlen; (inlen = sizeof(buf) * fread(buf, sizeof(buf), 1, fp)); ) {
            sha1_process(&hs, (const unsigned char *)buf, inlen);
        }
    }
    unsigned char *hash = va("%.*s", SHA1_HASHLEN, "");
    sha1_done(&hs, hash);
    return hash;
}

void* file_md5(const char *file) { // 16bytes
    hash_state hs = {0};
    md5_init(&hs);
    for( FILE *fp = fopen(file, "rb"); fp; fclose(fp), fp = 0) {
        char buf[8192];
        for( int inlen; (inlen = sizeof(buf) * fread(buf, sizeof(buf), 1, fp)); ) {
            md5_process(&hs, (const unsigned char *)buf, inlen);
        }
    }
    unsigned char *hash = va("%.*s", MD5_HASHLEN, "");
    md5_done(&hs, hash);
    return hash;
}

void* file_crc32(const char *file) { // 4bytes
    unsigned crc = 0;
    for( FILE *fp = fopen(file, "rb"); fp; fclose(fp), fp = 0) {
        char buf[8192];
        for( int inlen; (inlen = sizeof(buf) * fread(buf, sizeof(buf), 1, fp)); ) {
            crc = zip__crc32(crc, buf, inlen); // unsigned int stbiw__crc32(unsigned char *buffer, int len)
        }
    }
    unsigned char *hash = va("%.*s", (int)sizeof(crc), "");
    memcpy(hash, &crc, sizeof(crc));
    return hash;
}

#if 0
void* crc32_mem(const void *ptr, int inlen) { // 4bytes
    unsigned hash = 0;
    hash = zip__crc32(hash, ptr, inlen); // unsigned int stbiw__crc32(unsigned char *buffer, int len)
    return hash;
}
void* md5_mem(const void *ptr, int inlen) { // 16bytes
    hash_state hs = {0};
    md5_init(&hs);
    md5_process(&hs, (const unsigned char *)ptr, inlen);
    unsigned char *hash = va("%.*s", MD5_HASHLEN, "");
    md5_done(&hs, hash);
    return hash;
}
void* sha1_mem(const void *ptr, int inlen) { // 20bytes
    hash_state hs = {0};
    sha1_init(&hs);
    sha1_process(&hs, (const unsigned char *)ptr, inlen);
    unsigned char *hash = va("%.*s", SHA1_HASHLEN, "");
    sha1_done(&hs, hash);
    return hash;
}
unsigned crc32_mem(unsigned h, const void *ptr_, unsigned len) {
    // based on public domain code by Karl Malbrain
    const uint8_t *ptr = (const uint8_t *)ptr_;
    if (!ptr) return 0;
    const unsigned tbl[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
    for(h = ~h; len--; ) { uint8_t b = *ptr++; h = (h >> 4) ^ tbl[(h & 15) ^ (b & 15)]; h = (h >> 4) ^ tbl[(h & 15) ^ (b >> 4)]; }
    return ~h;
}
uint64_t crc64_mem(uint64_t h, const void *ptr, uint64_t len) {
    // based on public domain code by Lasse Collin
    // also, use poly64 0xC96C5795D7870F42 for crc64-ecma
    static uint64_t crc64_table[256];
    static uint64_t poly64 = UINT64_C(0x95AC9329AC4BC9B5);
    if( poly64 ) {
        for( int b = 0; b < 256; ++b ) {
            uint64_t r = b;
            for( int i = 0; i < 8; ++i ) {
                r = r & 1 ? (r >> 1) ^ poly64 : r >> 1;
            }
            crc64_table[ b ] = r;
            //printf("%016llx\n", crc64_table[b]);
        }
        poly64 = 0;
    }
    const uint8_t *buf = (const uint8_t *)ptr;
    uint64_t crc = ~h; // ~crc;
    while( len != 0 ) {
        crc = crc64_table[(uint8_t)crc ^ *buf++] ^ (crc >> 8);
        --len;
    }
    return ~crc;
}
// https://en.wikipedia.org/wiki/MurmurHash
static inline uint32_t murmur3_scramble(uint32_t k) {
    return k *= 0xcc9e2d51, k = (k << 15) | (k >> 17), k *= 0x1b873593;
}
uint32_t murmur3_mem(const uint8_t* key, size_t len, uint32_t seed) {
    uint32_t h = seed;
    uint32_t k;
    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
        // Here is a source of differing results across endiannesses.
        // A swap here has no effects on hash properties though.
        k = *((uint32_t*)key);
        key += sizeof(uint32_t);
        h ^= murmur3_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    /* Read the rest. */
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    // A swap is *not* necessary here because the preceeding loop already
    // places the low bytes in the low places according to whatever endianness
    // we use. Swaps only apply when the memory is copied in a chunk.
    h ^= murmur3_scramble(k);
    /* Finalize. */
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}
#endif

// -----------------------------------------------------------------------------
// storage (emscripten only)

void storage_mount(const char* folder) {
    #if is(ems)
        emscripten_run_script(va("FS.mkdir('%s'); FS.mount(IDBFS, {}, '%s');", folder, folder));
    #else
        (void)folder;
    #endif
}

void storage_read() {
    #if is(ems)
    EM_ASM(
        /* sync from persisted state into memory */
        FS.syncfs(true, function (err) {
            assert(!err);
        });
    );
    #endif
}

void storage_flush() {
    #if is(ems)
    EM_ASM(
        FS.syncfs(false, function (err) {
            assert(!err);
        });
    );
    #endif
}

// -----------------------------------------------------------------------------
// compressed archives

// return list of files inside zipfile
array(char*) file_zip_list(const char *zipfile) {
    static __thread array(char*) list[16] = {0};
    static __thread int count = 0;

    count = (count+1) % 16;
    array_resize(list[count], 0);

    for( zip *z = zip_open(zipfile, "rb"); z; zip_close(z), z = 0) {
        for( unsigned i = 0; i < zip_count(z); ++i ) {
            array_push( list[count], zip_name(z, i) );
        }
    }

    return list[count];
}

// extract single file content from zipfile
array(char) file_zip_extract(const char *zipfile, const char *filename) {
    static __thread array(char) list[16] = {0};
    static __thread int count = 0;

    array(char) out = list[count = (count+1) % 16];
    array_resize(out, 0);

    for( zip *z = zip_open(zipfile, "rb"); z; zip_close(z), z = 0) {
        int index = zip_find(z, filename); // convert entry to index. returns <0 if not found.
        if( index < 0 ) return zip_close(z), out;

        unsigned outlen = zip_size(z, index);
        unsigned excess = zip_excess(z, index);
        array_resize(out, outlen + 1 + excess);
        unsigned ret = zip_extract_inplace(z, index, out, array_count(out));
        if(ret) { out[outlen] = '\0'; array_resize(out, outlen); } else { array_resize(out, 0); }
    }

    return out;
}

// append single file into zipfile. compress with DEFLATE|6. Other compressors are also supported (try LZMA|5, ULZ|9, LZ4X|3, etc.)
bool file_zip_append(const char *zipfile, const char *filename, int clevel) {
    bool ok = false;
    for( zip *z = zip_open(zipfile, "a+b"); z; zip_close(z), z = 0) {
        for( FILE *fp = fopen(filename, "rb"); fp; fclose(fp), fp = 0) {
            ok = zip_append_file(z, filename, "", fp, clevel);
        }
    }
    return ok;
}

// append mem blob into zipfile. compress with DEFLATE|6. Other compressors are also supported (try LZMA|5, ULZ|9, LZ4X|3, etc.)
// @fixme: implement zip_append_mem() and use that instead
bool file_zip_appendmem(const char *zipfile, const char *entryname, const void *ptr, unsigned len, int clevel) {
    bool ok = false;
    if( ptr )
    for( zip *z = zip_open(zipfile, "a+b"); z; zip_close(z), z = 0) {
        ok = zip_append_mem(z, entryname, "", ptr, len, clevel);
    }
    return ok;
}

// ----------------------------------------------------------------------------
// ini


/* ini+, extended ini format
// - rlyeh, public domain
//
// # spec
//
//   ; line comment
//   [user]             ; map section name (optional)
//   name=john          ; key and value (mapped here as user.name=john)
//   +surname=doe jr.   ; sub-key and value (mapped here as user.name.surname=doe jr.)
//   age=30             ; numbers
//   color=240          ; key and value \
//   color=253          ; key and value |> array: color[0], color[1] and color[2]
//   color=255          ; key and value /
//   color=             ; remove key/value(s)
//   color=white        ; recreate key; color[1] and color[2] no longer exist
//   []                 ; unmap section
//   -note=keys may start with symbols (except plus and semicolon)
//   -note=linefeeds are either \r, \n or \r\n.
//   -note=utf8 everywhere.
*/

static
char *ini_parse( const char *s ) {
    char *map = 0;
    int mapcap = 0, maplen = 0;
    enum { DEL, REM, TAG, KEY, SUB, VAL } fsm = DEL;
    const char *cut[6] = {0}, *end[6] = {0};
    while( *s ) {
        while( *s && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') ) ++s;
        /**/ if( *s == ';' ) cut[fsm = REM] = ++s;
        else if( *s == '[' ) cut[fsm = TAG] = ++s;
        else if( *s == '+' ) cut[fsm = SUB] = ++s;
        else if( *s == '=' ) cut[fsm = VAL] = ++s;
        else if( *s > ' ' && *s <= 'z' && *s != ']' ) cut[fsm = KEY] = cut[SUB] = end[SUB] = s;
        else { if( *s ) ++s; continue; }
        /**/ if( fsm == REM ) { while(*s && *s != '\r'&& *s != '\n') ++s; }
        else if( fsm == TAG ) { while(*s && *s != '\r'&& *s != '\n'&& *s != ']') ++s; end[TAG] = s; }
        else if( fsm == KEY ) { while(*s && *s >  ' ' && *s <= 'z' && *s != '=') ++s; end[KEY] = s; }
        else if( fsm == SUB ) { while(*s && *s >  ' ' && *s <= 'z' && *s != '=') ++s; end[SUB] = s; }
        else if( fsm == VAL ) { while(*s && *s >= ' ' && *s <= 127 && *s != ';') ++s; end[VAL] = s;
            while( end[VAL][-1] <= ' ' ) { --end[VAL]; }
            char buf[256] = {0}, *key = buf;
            if( end[TAG] - cut[TAG] ) key += sprintf(key, "%.*s.", (int)(end[TAG] - cut[TAG]), cut[TAG] );
            if( end[KEY] - cut[KEY] ) key += sprintf(key,  "%.*s", (int)(end[KEY] - cut[KEY]), cut[KEY] );
            if( end[SUB] - cut[SUB] ) key += sprintf(key, ".%.*s", (int)(end[SUB] - cut[SUB]), cut[SUB] );
            int reqlen = (key - buf) + 1 + (end[VAL] - cut[VAL]) + 1 + 1;
            if( (reqlen + maplen) >= mapcap ) map = REALLOC( map, mapcap += reqlen + 512 );
            sprintf( map + maplen, "%.*s%c%.*s%c%c", (int)(key - buf), buf, 0, (int)(end[VAL] - cut[VAL]), cut[VAL], 0, 0 );
            maplen += reqlen - 1;
        }
    }
    return map;
}

// @todo: evaluate alt api
// int count = ini_count(filename);
// char *key = ini_key(filename, id);
// char *val = ini_val(filename, id);

void ini_destroy(ini_t x) {
    for each_map(x, char*, k, char*, v) {
        FREE(k);
        FREE(v);
    }
    map_free(x);
}

ini_t ini_from_mem(const char *data) {
    if( !data || !data[0] ) return 0;

    char *kv = ini_parse(data);
    if( !kv ) return 0;

        ini_t map = 0;
        map_init(map, less_str, hash_str);
        for( char *iter = kv; iter[0]; ) {
            char *key = iter; while( *iter++ );
            char *val = iter; while( *iter++ );

            char **found = map_find(map, key);
            if( !found ) map_insert(map, STRDUP(key), STRDUP(val));
            assert( map_find(map,key) );
        }

    FREE( kv );

    return map;
}

ini_t ini(const char *filename) {
    char *kv = file_read(filename);
    if(!kv) kv = vfs_read(filename);
    return ini_from_mem(kv);
}

bool ini_write(const char *filename, const char *section, const char *key, const char *value) {
    // this is a little hacky {
    char *data = file_read(filename);
    if( data && data[0] ) {
        char *begin = strrchr(data, '[');
        char *end = strrchr(data, ']');
        if( begin && end && begin < end ) {
            char *last_section = va("%.*s", (int)(end - begin - 1), begin + 1);
            if( !strcmpi(last_section, section) ) section = 0;
        }
    }
    // }

    char *s = va("%s%s=%s\r\n", section ? va("[%s]\r\n", section) : "", key, value);
    return file_append(filename, s, strlen(s));
}

