// -----------------------------------------------------------------------------
// archives

enum { is_zip, is_tar, is_pak, is_dir };

typedef struct archive_dir {
    char* path;
    union {
        int type;
        int size; // for cache only
    };
    union {
        void *archive;
        void *data; // for cache only
        zip* zip_archive;
        tar* tar_archive;
        pak* pak_archive;
    };
    struct archive_dir *next;
} archive_dir;

static archive_dir *dir_mount;
static archive_dir *dir_cache;

#ifndef MAX_CACHED_FILES     // @todo: should this be MAX_CACHED_SIZE (in MiB) instead?
#define MAX_CACHED_FILES 256 // @todo: should we cache the cooked contents instead? ie, stbi() result instead of file.png?
#endif

struct vfs_entry {
    const char *name;
    const char *id;
    unsigned size;
};
static array(struct vfs_entry) vfs_hints;   // mounted raw assets
static array(struct vfs_entry) vfs_entries; // mounted cooked assets

static bool vfs_mount_hints(const char *path);

void vfs_reload() {
    const char *app = app_name();

    array_resize(vfs_hints, 0); // @leak
    array_resize(vfs_entries, 0); // @leak

    // mount virtual filesystems later (mounting order matters: low -> to -> high priority)
#if defined(EMSCRIPTEN)
    vfs_mount("index.zip");
#else
    // mount fused executables
    vfs_mount(va("%s%s%s", app_path(), app_name(), ifdef(win32, ".exe", "")));
    // mount all zipfiles
    for each_array( file_list("*.zip"), char*, file ) vfs_mount(file);
#endif

#if HAS_COOK
    // vfs_resolve() will use these art_folder locations as hints when cook-on-demand is in progress.
    // cook-on-demand will not be able to resolve a virtual pathfile if there are no cooked assets on disk,
    // unless there is a record of what's actually on disk somewhere, and that's where the hints belong to.
    if( COOK_ON_DEMAND )
    for each_substring(ART,",",art_folder) {
        vfs_mount_hints(art_folder);
    }
#endif
}



#define ARK1         0x41724B31 // 'ArK1' in le, 0x314B7241 41 72 4B 31 otherwise
#define ARK1_PADDING (512 - 40) // 472
#define ARK_PRINTF(f,...) 0 // printf(f,__VA_ARGS__)
#define ARK_SWAP32(x) (x)
#define ARK_SWAP64(x) (x)
#define ARK_REALLOC   REALLOC
static uint64_t ark_fget64( FILE *in ) { uint64_t v; fread( &v, 8, 1, in ); return ARK_SWAP64(v); }
void ark_list( const char *infile, zip **z ) {
    for( FILE *in = fopen(infile, "rb"); in; fclose(in), in = 0 )
    while(!feof(in)) {
        if( 0 != (ftell(in) % ARK1_PADDING) ) fseek(in, ARK1_PADDING - (ftell(in) % ARK1_PADDING), SEEK_CUR);
        ARK_PRINTF("Reading at #%d\n", (int)ftell(in));
        uint64_t mark = ark_fget64(in);
        if( mark != ARK1 ) continue;
        uint64_t stamp = ark_fget64(in);
        uint64_t datalen = ark_fget64(in);
        uint64_t datahash = ark_fget64(in);
        uint64_t namelen = ark_fget64(in);

        *z = zip_open_handle(in, "rb");
        return;
    }
}



static
bool vfs_mount_(const char *path, array(struct vfs_entry) *entries) {
    const char *path_bak = path;

    zip *z = NULL; tar *t = NULL; pak *p = NULL; dir *d = NULL;
    int is_folder = ('/' == path[strlen(path)-1]);
    if( is_folder ) d = dir_open(path, "rb");
    if( is_folder && !d ) return 0;
    if( !is_folder ) z = zip_open(path, "rb");
    if( !is_folder && !z ) t = tar_open(path, "rb");
    if( !is_folder && !z && !t ) p = pak_open(path, "rb");
    if( !is_folder && !z && !t && !p ) ark_list(path, &z); // last resort. try as .ark
    if( !is_folder && !z && !t && !p ) return 0;

    // normalize input -> "././" to ""
    while (path[0] == '.' && path[1] == '/') path += 2;
    path = STRDUP(path);
    if( z || t || p ) {
    // save local path for archives, so we can subtract those from upcoming requests
    if(strrchr(path,'/')) strrchr(path,'/')[1] = '\0';
    } else if(d) 0[(char*)path] = 0;

    // append to mounted archives
    archive_dir *prev = dir_mount, zero = {0};
    *(dir_mount = REALLOC(0, sizeof(archive_dir))) = zero;
    dir_mount->next = prev;
    dir_mount->path = (char*)path;
    dir_mount->archive = z ? (void*)z : t ? (void*)t : p ? (void*)p : (void*)d;
    dir_mount->type = is_folder ? is_dir : z ? is_zip : t ? is_tar : p ? is_pak : -1;
    ASSERT(dir_mount->type >= 0 && dir_mount->type < 4);

    // append list of files to internal listing
    for( archive_dir *dir = dir_mount; dir ; dir = 0 ) { // for(archive_dir *dir = dir_mount; dir; dir = dir->next) {
        assert(dir->type >= 0 && dir->type < 4);
        unsigned (*fn_count[4])(void*) = {(void*)zip_count, (void*)tar_count, (void*)pak_count, (void*)dir_count};
        char*    (*fn_name[4])(void*, unsigned index) = {(void*)zip_name, (void*)tar_name, (void*)pak_name, (void*)dir_name};
        unsigned (*fn_size[4])(void*, unsigned index) = {(void*)zip_size, (void*)tar_size, (void*)pak_size, (void*)dir_size};

        for( unsigned idx = 0, end = fn_count[dir->type](dir->archive); idx < end; ++idx ) {
            assert(idx < end);
            const char *filename = STRDUP( fn_name[dir->type](dir->archive, idx) );
            const char *fileid = STRDUP( file_id(filename) );
            unsigned filesize = fn_size[dir->type](dir->archive, idx);
            // printf("%u) %s %u [%s]\n", idx, filename, filesize, fileid);
            // append to list
            array_push(*entries, (struct vfs_entry){filename, fileid, filesize});
        }

        // PRINTF("Mounted VFS volume '%s' (%u entries)\n", path_bak, fn_count[dir->type](dir->archive) );
    }

    return 1;
}
static
bool vfs_mount_hints(const char *path) {
    return vfs_mount_(path, &vfs_hints);
}
bool vfs_mount(const char *path) {
    return vfs_mount_(path, &vfs_entries);
}
array(char*) vfs_list(const char *masks) {
    static __thread array(char*) list = 0; // @fixme: add 16 slots

    for( int i = 0; i < array_count(list); ++i ) {
        FREE(list[i]);
    }
    array_resize(list, 0);

    for each_substring(masks,";",it) {
#if HAS_COOK
        if( COOK_ON_DEMAND ) // edge case: any game using only vfs api + cook-on-demand flag will never find any file
        for each_array(file_list(it), char*, item) {
            // insert copy
            char *copy = STRDUP(item);
            array_push(list, copy);
        }
#endif

        it = va("*/%s", it);
        // int recurse = !!strstr(it, "**"); // @fixme: support non-recursive
        for( unsigned i = 0; i < array_count(vfs_entries); ++i ) {
            const char *name = vfs_entries[i].name;
            if( strmatch(name, it) ) {
                // insert copy
                char *copy = STRDUP(name);
                array_push(list, copy);
            }
        }
    }

    // sort alphabetically then remove dupes
    array_sort(list, strcmp);
    array_unique(list, strcmp_qsort);

    return list;
}

static
char *vfs_unpack(const char *pathfile, int *size) { // must FREE() after use
    // @todo: add cache here
    char *data = NULL;
    for(archive_dir *dir = dir_mount; dir && !data; dir = dir->next) {
        if( dir->type == is_dir ) {
#if 0 // sandboxed
            char buf[DIR_MAX];
            snprintf(buf, sizeof(buf), "%s%s", dir->path, pathfile);
            data = file_load(buf, size);
#endif
        } else {
            int      (*fn_find[3])(void *, const char *) = {(void*)zip_find, (void*)tar_find, (void*)pak_find};
            void*    (*fn_unpack[3])(void *, unsigned) = {(void*)zip_extract, (void*)tar_extract, (void*)pak_extract};
            unsigned (*fn_size[3])(void *, unsigned) = {(void*)zip_size, (void*)tar_size, (void*)pak_size};

#if 0
            const char* cleanup = pathfile + strbegi(pathfile, dir->path) * strlen(dir->path);
            while (cleanup[0] == '/') ++cleanup;
#else
            const char *cleanup = pathfile;
#endif

            int index = fn_find[dir->type](dir->archive, cleanup);
            data = fn_unpack[dir->type](dir->archive, index);
            if( size ) *size = fn_size[dir->type](dir->archive, index);
        }
        // printf("%c trying %s in %s ...\n", data ? 'Y':'N', pathfile, dir->path);
    }
    //wait_ms(1000); // <-- simulate slow hdd
    return data;
}

const char *vfs_resolve(const char *pathfile) {
    // we dont resolve absolute paths. they dont belong to the vfs
    // if( pathfile[0] == '/' || pathfile[0] == '\\' || pathfile[1] == ':' ) return pathfile;

    char* id = file_id(pathfile);
    // find best match (vfs_entries first)
    for (int i = array_count(vfs_entries); --i >= 0; ) {
        if (strbegi(vfs_entries[i].id, id) ) {
            return vfs_entries[i].name;
        }
    }
    // find best match (vfs_hints later)
    for (int i = array_count(vfs_hints); --i >= 0; ) {
        if (strbegi(vfs_hints[i].id, id) ) {
            return vfs_hints[i].name;
        }
    }
    return pathfile;
}

char* vfs_load(const char *pathfile, int *size_out) { // @todo: fix leaks, vfs_unpack()
    if (!dir_mount) {
        return file_load(pathfile, size_out);
    }

    // @fixme: handle \\?\ absolute path (win)
    if (!pathfile[0]) return file_load(pathfile, size_out);
    while( pathfile[0] == '.' && (pathfile[1] == '/' || pathfile[1] == '\\') ) pathfile += 2;
    // if (pathfile[0] == '/' || pathfile[1] == ':') return file_load(pathfile, size_out); // @fixme: handle current cooked /home/ENGINE or C:/ENGINE path cases within zipfiles

if( size_out ) *size_out = 0;
if( strend(pathfile, "/") ) return 0; // it's a dir
static __thread map(char*,int) misses = 0, *init = 0; if(!init) init = misses, map_init(misses, less_str, hash_str);
int *found = map_find_or_add_allocated_key(misses, STRDUP(pathfile), -1); // [-1]non-init,[false]could not cook,[true]cooked
if( found && *found == 0 ) {
    return 0;
}

    //{
    // exclude garbage from material names
    // @todo: exclude double slashs in paths
    char *base = file_name(pathfile); if(strchr(base,'+')) base = strchr(base, '+')+1;
    if(base[0] == '\0') return 0; // it's a dir
    char *folder = file_path(pathfile);
    pathfile = va("%s%s", folder, base);

    // solve virtual path
    pathfile = va("%s", vfs_resolve(pathfile));
    base = file_name(pathfile);
    if(base[0] == '\0') return 0; // it's a dir
    folder = file_path(pathfile);
        char* pretty_folder = "";
#if HAS_COOK
        // ease folders reading by shortening them: /home/rlyeh/project/art/demos/audio/coin.wav -> demos/audio/coin.wav
        // or C:/project/engine/art/fonts/B612-BoldItalic.ttf -> fonts/B612-BoldItalic.ttf
        static __thread array(char*) art_paths = 0;
        if(!art_paths) for each_substring(ART,",",stem) array_push(art_paths, STRDUP(stem));
        if( folder ) for( int i = 0; i < array_count(art_paths); ++i ) {
            if( strbeg(folder, art_paths[i]) ) { pretty_folder = folder + strlen(art_paths[i]); break; }
        }
#endif
    //}

    int size = 0;
    void *ptr = 0;

#if 0
    // clean pathfile
    while (pathfile[0] == '.' && pathfile[1] == '/') pathfile += 2;
    while (pathfile[0] == '/') ++pathfile;
#endif

    const char *lookup_id = /*file_normalize_with_folder*/(pathfile);

    // search (last item)
    static __thread char  last_item[256] = { 0 };
    static __thread void *last_ptr = 0;
    static __thread int   last_size = 0;
    if( !strcmpi(lookup_id, last_item)) {
        ptr = last_ptr;
        size = last_size;
    }

    // search (cache)
    if( !ptr && !is(osx) ) { // @todo: remove silicon mac M1 hack
        ptr = cache_lookup(lookup_id, &size);
    }

    if( ptr ) {
        PRINTF("Loading VFS (%s)%s (cached)\n", pretty_folder, base);
    } else {
        PRINTF("Loading VFS (%s)%s\n", pretty_folder, base);
    }

    // read cooked asset from mounted disks
    if( !ptr ) {
        ptr = vfs_unpack(pathfile, &size);

#if HAS_COOK
        // asset not found? maybe it has not been cooked yet at this point (see --cook-on-demand)
        if( !ptr && COOK_ON_DEMAND ) {

            static thread_mutex_t mutex, *init = 0; if(!init) thread_mutex_init(init = &mutex);
            thread_mutex_lock(&mutex);

// this block saves some boot time (editor --cook-on-demand: boot 1.50s -> 0.90s)
#if 1 // EXPERIMENTAL_DONT_COOK_NON_EXISTING_ASSETS
            static set(char*) disk = 0;
            if(!disk) { set_init_str(disk); for each_substring(ART,",",art_folder) for each_array(file_list(va("%s**", art_folder)), char*, item) set_insert(disk, STRDUP(item)); } // art_folder ends with '/'
            int found = !!set_find(disk, (char*)pathfile);
            if( found )
#endif
            {
                // technically, we should only cook `base` asset at this point. however, cooks on demand can be very
                // expensive. not only because we're invoking an external tool/cook app in here, which is scanning all the
                // cook folders at every call, but also because there is this expensive vfs_reload() call at end of current scope.
                // so, in order to minimize the number of upcoming cook requests, we cook more stuff than needed at this point;
                // just in anticipation of what's likely going to happen in the next frames.
                // so, when asked to cook "models/model.fbx" we actually:
                // - do cook "models/model.* (so, "model.tga","model.png","model.jpg","model.mtl",etc. are also cooked)
                // - do cook "models/*.fbx" as well
                char *dir = file_path(pathfile + ART_SKIP_ROOT);
                char *group1 = dir[0] ? va("\"*/%s%s.*\"", dir, file_base(pathfile)) : base; // -> "*/models/model.*"
                char *group2 = dir[0] ? va("\"*/%s*%s\"", dir, file_ext(pathfile)) : ""; // -> "*/models/*.fbx"

                char *cmd = va("%scook" ifdef(osx,".osx",ifdef(linux,".linux",".exe"))" %s %s --cook-ini=%s --cook-additive --cook-jobs=1 --quiet", TOOLS, group1, group2, COOK_INI);

                // cook groups
                int rc = system(cmd); // atoi(app_exec(cmd));
                if(rc < 0) PANIC("cannot invoke `%scook` (return code %d)", TOOLS, rc);

                vfs_reload(); // @todo: optimize me. it is waaay inefficent to reload the whole VFS layout after cooking a single asset
            }

            thread_mutex_unlock(&mutex);

            // finally, try again
            pathfile = va("%s", vfs_resolve(pathfile));
            ptr = vfs_unpack(pathfile, &size);
        }
#endif

        if( ptr ) {
            cache_insert(lookup_id, ptr, size);
        }
    }

    if( ptr && size )
    if( ptr != last_ptr) {
        snprintf(last_item, 256, "%s", lookup_id);
        last_ptr = ptr;
        last_size = size;
    }

    // yet another last resort: redirect vfs_load() calls to file_load()
    // (for environments without tools or cooked assets)
    if(!ptr) {
        if( !have_tools() ) {
            ptr = file_load(pathfile, size_out);
        }
    }

    if(!ptr) {
        PRINTF("Loading %s (not found)\n", pathfile);
    }

*found = ptr ? true : false;

    if( size_out ) *size_out = ptr ? size : 0;
    return ptr;
}
char* vfs_read(const char *pathfile) {
    return vfs_load(pathfile, NULL);
}
int vfs_size(const char *pathfile) {
    int sz;
    return vfs_load(pathfile, &sz), sz;
}


FILE* vfs_handle(const char *pathfile) {
    // @fixme: non-compressible assets (adpcm wavs,mp3,ogg,mp4,avi,...) are taking a suboptimal code path here.
    //         no need to unzip them. just seek within the zipfile and return the *fp at that position
    int sz;
    char *buf = vfs_load(pathfile, &sz);
    FILE *fp = fmemopen(buf ? buf : "", buf ? sz : 0, "r+b");
    ASSERT( fp, "cannot create tempfile" );
    return fp;
}
#if 0
const char *vfs_extract(const char *pathfile) { // extract a vfs file into the local (temp) filesystem
#if 0
    FILE* fp = vfs_handle(pathfile);
    return fp ? pathfile_from_handle(fp) : "";
#else
    int sz;
    char *buf = vfs_load(pathfile, &sz);
    if( !buf ) return "";

    // pool of temp files. recycles after every loop
    enum { MAX_TEMP_FILES = 16 };
    static __thread char temps[MAX_TEMP_FILES][DIR_MAX] = {0};
    static __thread int i = 0;
    if( temps[i][0] ) unlink(temps[i]);
    i = (i + 1) % MAX_TEMP_FILES;
    if(!temps[i][0] ) snprintf(temps[i], DIR_MAX, "%s", file_tempname());
    char *name = temps[i];

    FILE *tmp = fopen(name, "wb"); //unlink(name);
    ASSERT( tmp, "cannot create tempfile %s", name );
    fwrite(buf ? buf : "", 1, buf ? sz : 0, tmp);
    fclose(tmp);
    return name;
#endif
}
#endif


// -----------------------------------------------------------------------------
// cache

static thread_mutex_t cache_mutex; AUTORUN{ thread_mutex_init(&cache_mutex); }

void* cache_lookup(const char *pathfile, int *size) { // find key->value
    if( !MAX_CACHED_FILES ) return 0;
    void* data = 0;
    thread_mutex_lock(&cache_mutex);
    for(archive_dir *dir = dir_cache; dir; dir = dir->next) {
        if( !strcmp(dir->path, pathfile) ) {
            if(size) *size = dir->size;
            data = dir->data;
            break;
        }
    }
    thread_mutex_unlock(&cache_mutex);
    return data;
}
void* cache_insert(const char *pathfile, void *ptr, int size) { // append key/value; return LRU or NULL
    if( !MAX_CACHED_FILES ) return 0;
    if( !ptr || !size ) return 0;

    // keep cached files within limits
    thread_mutex_lock(&cache_mutex);

        // append to cache
        archive_dir zero = {0}, *old = dir_cache;
        *(dir_cache = REALLOC(0, sizeof(archive_dir))) = zero;
        dir_cache->next = old;
        dir_cache->path = STRDUP(pathfile);
        dir_cache->size = size;
        dir_cache->data = REALLOC(0, size+1);
        memcpy(dir_cache->data, ptr, size); size[(char*)dir_cache->data] = 0; // copy+terminator

        void *found = 0;

        static int added = 0;
        if( added < MAX_CACHED_FILES ) {
            ++added;
        } else {
            // remove oldest cache entry
            for( archive_dir *prev = dir_cache, *dir = prev; dir ; prev = dir, dir = dir->next ) {
                if( !dir->next ) {
                    prev->next = 0; // break link
                    found = dir->data;
                    dir->path = REALLOC(dir->path, 0);
                    dir->data = REALLOC(dir->data, 0);
                    dir = REALLOC(dir, 0);
                    break;
                }
            }
        }

    thread_mutex_unlock(&cache_mutex);

    return found;
}
