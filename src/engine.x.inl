/* game framework (3rd libs)
 *  - rlyeh, public domain
 */

#ifdef ENGINE_3RD

//-----------------------------------------------------------------------------
// 3rd party libs












//---
#define SWRAP_IMPLEMENTATION                  // swrap
#define SWRAP_STATIC                          // swrap
{{FILE:3rd_swrap.h}}
//---
#define JO_MPEG_COMPONENTS 3                  // jo_mpeg
{{FILE:3rd_jo_mp1.h}}
#define get_bits stb_vorbis_get_bits
#define error stb_vorbis_error
{{FILE:3rd_stb_vorbis.h}}
#undef error
#undef DEBUG
#define STS_MIXER_IMPLEMENTATION              // sts_mixer
{{FILE:3rd_sts_mixer.h}}

#define MINIAUDIO_IMPLEMENTATION              // miniaudio
#define MA_NO_FLAC                            // miniaudio
#ifdef __APPLE__
#define MA_NO_RUNTIME_LINKING                 // miniaudio osx
#endif
{{FILE:3rd_miniaudio.h}}

//---
#undef L
#undef C
#undef R
#define error l_error
#define panic l_panic
{{FILE:3rd_lua.h}}
#undef cast
#undef G
//---
//---
#ifdef ENABLE_ASSIMP
//{{FILE/*:*/3rd_assimp.h}}
//#include "3rd_assimp/cimport.h"
//#include "3rd_assimp/scene.h"
//#include "3rd_assimp/postprocess.h"
//#pragma comment(lib, "3rd/3rd_assimp/x64/assimp")
#endif

{{FILE:3rd_plmpeg.h}}
{{FILE:3rd_jo_mpeg.h}}

{{FILE:3rd_https.h}}
#undef F2
#undef F3
#define ENET_IMPLEMENTATION                   // enet
{{FILE:3rd_enet.h}}
#define tls_init tls_init2
#define BQ_PLATFORM_IMPLEMENTATION            // websocket
#define BQ_WEBSOCKET_IMPLEMENTATION           // websocket
{{FILE:3rd_bq_websocket.h}}


#define STB_SPRINTF_IMPLEMENTATION            // stb_sprintf
#define STB_SPRINTF_NOUNALIGNED               // stb_sprintf
{{FILE:3rd_stb_sprintf.h}}

#define XML_C                                 // xml
#define g g2
{{FILE:3rd_xml.h}}
#undef g

{{FILE:3rd_polychop.h}}


#define BASE64_C                              // base64.c
{{FILE:3rd_base64.h}}

#if ENABLE_RPMALLOC
{{FILE:3rd_rpmalloc.h}}
{{FILE:3rd_rpmalloc.c}}
//{{FILE: 3rd_rpmalloci.c}}
#define SYS_MEM_INIT()   rpmalloc_initialize()
#define SYS_MEM_REALLOC  rprealloc
#define SYS_MEM_SIZE     rpmalloc_usable_size
#endif

//#define SQLITE_OMIT_LOAD_EXTENSION
//#define SQLITE_CORE 1
//#define SQLITE_DEBUG 1
//#define Token SQToken
//#define Table SQTable
//#define rehash sqlite3__rehash
//#undef NB
//{ {FILE:3rd_sqlite3.c}}
//#undef Token
//#undef Table
//#undef rehash
//#undef NB
//#undef threadid

// editor_script
#define GLEQ_IMPLEMENTATION
#define GlyphSet GlyphSet_ // linux
{{FILE:3rd_lite_sys_gleq.h}}
{{FILE:3rd_lite_sys.h}}
{{FILE:3rd_lite.h}}

#define LIGHTMAPPER_IMPLEMENTATION
//#define LM_DEBUG_INTERPOLATION
{{FILE:3rd_lightmapper.h}}

{{FILE:3rd_sdk_steam.h}}

#endif // ENGINE_3RD






//-----------------------------------------------------------------------------
// 3rd party libs

#define TIGR_C
//#define TIGR_DO_NOT_PRESERVE_WINDOW_POSITION // @fixme: make it centered
#define run run2
#define border border2
#include "3rd/tigr/3rd_tigr.h"
#include "3rd/tigr/3rd_tigrobjc.h"
#include "3rd/tigr/3rd_tigrmousecursor.h"
#include "3rd/tigr/3rd_tigrdragndrop.h"
#include "3rd/tigr/3rd_tigrtitle.h"
#undef border
#undef run

#if 0
#define LUA_IMPL                              // lua544
#define TK_END TK_END2
#define TK_RETURN TK_RETURN2
#define block block2
#include "3rd/3rd_lua.h"
#undef TK_END
#undef TK_RETURN
#endif

#define SOKOL_AUDIO_IMPL
#include "3rd/sokol/3rd_sokol_audio.h"

#define DEFLATE_C
#include "3rd/3rd_compress_deflate.h"
#include "3rd/3rd_archive_zlib.h" // zlib streams
#include "3rd/3rd_archive_gz.h" // gunzip

#include "3rd/3rd_bin.h"
