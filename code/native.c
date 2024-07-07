#include "engine.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#define STB_SPRINTF_IMPLEMENTATION

#define TFD_IMPLEMENTATION
#pragma comment(lib, "ole32")
#pragma comment(lib, "comdlg32")

#define XML_C
#define JSON5_C
#define BASE64_C
#define DIR_C
#define ZIP_C
#define TAR_C
#define PAK_C
#define COMPRESS_C
#define DEFLATE_C
#define SWRAP_IMPLEMENTATION                  // swrap
//#define SWRAP_STATIC                          // swrap

#define EVAL_C
#define EVAL_EXTEND_CONSTANTS \
   for( int vk = input_enum(id), *once = &vk; once; once = 0) \
      if( vk >= 0 ) push(ev, vk);
#define EVAL_EXTEND_FUNCTIONS \
   /**/ if(nargs == 1 && !strcmp(id, "input") ) push(ev, input(pop(ev))); \
   else if(nargs == 1 && !strcmp(id, "down")  ) push(ev, input_down(pop(ev))); \
   else if(nargs == 1 && !strcmp(id, "hold")  ) push(ev, input_held(pop(ev))); \
   else if(nargs == 1 && !strcmp(id, "held")  ) push(ev, input_held(pop(ev))); \
   else if(nargs == 1 && !strcmp(id, "up")    ) push(ev, input_up(pop(ev))); \
   else if(nargs == 1 && !strcmp(id, "idle")  ) push(ev, input_idle(pop(ev)));
//#define expr expr2 // 3rd_eval.h + 3rd_lua.h conflict

#include "3rd/3rd.h"

// @fixme: convert these two into headers (they're not)
#include "3rd/3rd_compress_zlib.h"
#include "3rd/3rd_archive_gz.h"

#define GLAD_GL_IMPLEMENTATION
#include <3rd_glad.h>

#if is(win32) && (is(cl) || is(tcc))
#pragma comment(lib, "ws2_32") // swrap
#endif

#include "3rd/3rd_stb_vorbis.c"

