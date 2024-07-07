// -----------------------------------------------------------------------------
// cache and virtual filesystem (registered directories and/or compressed zip archives).
// - rlyeh, public domain.
//
// - note: vfs_mount() order matters (last mounts have higher priority).
// - note: directory/with/trailing/slash/ as mount_point, or zip/tar/pak archive otherwise.

// virtual filesystem

API bool         vfs_mount(const char *mount_point);
API array(char*) vfs_list(const char *masks); // **.png;*.c

API char *       vfs_read(const char *pathfile);
API char *       vfs_load(const char *pathfile, int *size);
API int          vfs_size(const char *pathfile);

API void         vfs_reload();
API const char * vfs_resolve(const char *fuzzyname); // guess best match. @todo: fuzzy path
//API const char*vfs_extract(const char *pathfile); // extracts vfs file into local filesystem (temporary file), so it can be read by foreign/3rd party libs
API FILE*        vfs_handle(const char *pathfile); // same as above, but returns file handle instead. preferred way, will clean descriptors at exit

// cache

API void *       cache_insert(const char *key, void *value, int size);
API void *       cache_lookup(const char *key, int *size);
