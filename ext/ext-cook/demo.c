#include "engine.h"

int main() {
    double then = time_ss();

    unsigned zlevel = optioni("--level,-l", 1); // [0..15]
    unsigned zflags = optioni("--compressor,-c", COMPRESS_ULZ); // 0 for deflate

    // zipfile can be null: --zipfile= 
    // if zipfile is null, no zipfile will be written.
    const char* zipfile = option("--zipfile,-z", "cook.zip");
    zip_t* z = zipfile ? zip_open(zipfile, "wb") : NULL;

    int count;
    char **list = file_list(option("--input,-i", "art/**;assets/**;data/**;demos/**;embed/**;ext/**/embed/*;ext/**/demo/*;"), &count);
    while( count-- ) {
        // skip cook process if file is already cooked
        if( file_name(list[count])[0] == '.' ) continue;

        // cook & read
        int size;
        char *cooked = file_read(list[count], &size);

        // append cooked file to zip, if needed
        if( z ) {
            zip_append_mem(z, list[count], ""/*comment*/, cooked, size, zflags | zlevel);
        }
    }

    if(z) zip_close(z);

    printf("%5.2fs\n", time_ss() - then);
}
