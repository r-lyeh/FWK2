// -----------------------------------------------------------------------------
// ffmpeg video recording
// [src] http://blog.mmacklin.com/2013/06/11/real-time-video-capture-with-ffmpeg/
// -----------------------------------------------------------------------------
// video recorder (uses external ffmpeg and fallbacks to built-in mpeg1 encoder)
// - rlyeh, public domain
//
// @fixme: MSAA can cause some artifacts with Intel PBOs: either use glDisable(GL_MULTISAMPLE) before recording or do not create window with WINDOW_MSAA at all.
//
// @todo: RECORD_UI (reasonable?)

#if !CODE

typedef enum RECORD_FLAGS {
    RECORD_MOUSE = 1,
} RECORD_FLAGS;

API bool       record_start(const char *outfile_mp4, unsigned flags);
API bool        record_active();
API void       record_stop(void);

#else

#define JO_MPEG_COMPONENTS 3                  // jo_mpeg
#include "3rd_jo_mpeg.h"

static FILE* rec_ffmpeg;
static FILE* rec_mpeg1;
static unsigned rec_flags;

void record_stop(void) {
    if(rec_ffmpeg) ifdef(win32, _pclose, pclose)(rec_ffmpeg);
    rec_ffmpeg = 0;

    if(rec_mpeg1) fclose(rec_mpeg1);
    rec_mpeg1 = 0;
}

bool record_active() {
    return rec_ffmpeg || rec_mpeg1;
}

bool record_start(const char *outfile_mp4, unsigned flags) {
    do_once atexit(record_stop);

    record_stop();

    rec_flags = flags;

    const char *ffmpeg_exe = ifdef(win32, "ext\\ext-ffmpeg\\ffmpeg", "ext/ext-ffmpeg/ffmpeg")
        ifdef(win32, ".exe", ifdef(osx, ".osx", ".linux"));

    // first choice: external ffmpeg encoder
    if( !rec_ffmpeg && is_file(ffmpeg_exe) ) {

        char *cmd = va("%s "
                    "-hide_banner -loglevel error " // less verbose
                    "-r %d -f rawvideo -pix_fmt bgr24 -s %dx%d " // raw BGR WxH-60Hz frames
                    // "-framerate 30 " // interpolating new video output frames from the source frames
                    "-i - "              // read frames from stdin
                    //"-draw_mouse 1 "
                    "-threads 0 "
                    //"-vsync vfr "
                    "-preset ultrafast " // collection of options that will provide a certain encoding speed [fast,ultrafast]
                    // "-tune zerolatency " // change settings based upon the specifics of your input
                    //"-crf 21 "           // range of the CRF scale [0(lossless)..23(default)..51(worst quality)]
                    "-pix_fmt yuv420p "  // compatible with Windows Media Player and Quicktime
                    "-vf vflip "         // flip Y
//                  "-vf \"pad=ceil(iw/2)*2:ceil(ih/2)*2\" "
                    "-y \"%s\"", ffmpeg_exe,
                    (int)fps(), app_width(), app_height(), outfile_mp4);    // overwrite output file

        // -rtbufsize 100M (https://trac.ffmpeg.org/wiki/DirectShow#BufferingLatency) Prevent some frames in the buffer from being dropped.
        // -probesize 10M (https://www.ffmpeg.org/ffmpeg-formats.html#Format-Options) Set probing size in bytes, i.e. the size of the data to analyze to get stream information. A higher value will enable detecting more information in case it is dispersed into the stream, but will increase latency. Must be an integer not lesser than 32. It is 5000000 by default.
        // -c:v libx264 (https://www.ffmpeg.org/ffmpeg.html#Main-options) Select an encoder (when used before an output file) or a decoder (when used before an input file) for one or more streams. codec is the name of a decoder/encoder or a special value copy (output only) to indicate that the stream is not to be re-encoded.

        // open pipe to ffmpeg's stdin in binary write mode
        rec_ffmpeg = ifdef(win32, _popen(cmd, "wb"), popen(cmd, "w"));
    }

    // fallback: built-in mpeg1 encoder
    if( !rec_ffmpeg ) {
        PRINTF("ffmpeg not found: using mpeg-1 fallback (hint: consider installing ext-ffmpeg)\n");
        rec_mpeg1 = fopen(outfile_mp4, "wb"); // "a+b"
    }

    return record_active();
}

void record_frame() {
    if( record_active() ) {
        void* pixels = screenshot_async(-3); // 3 RGB, 4 RGBA, -3 BGR, -4 BGRA. ps: BGR is fastest on my intel discrete gpu

        if( rec_ffmpeg ) {
            fwrite(pixels, 3 * app_width() * app_height(), 1, rec_ffmpeg);
        }
        if( rec_mpeg1 ) {
            jo_write_mpeg(rec_mpeg1, pixels, app_width(), app_height(), 24);  // 24fps
        }
    }
}

// int window_record(const char *outfile_mp4) {
//     record_start(outfile_mp4);
//     // @todo: if( flags & RECORD_MOUSE )
//     if( record_active() ) window_cursor_shape(CURSOR_SW_AUTO); else window_cursor_shape(CURSOR_HW_ARROW);
//     return record_active();
// }

static void record_tick() {
    static unsigned wait_frames = 0;
    if( record_active() ) {
        if( rec_flags & RECORD_MOUSE ) {
            // draw mouse shape if requested
            // platform with software cursors (like consoles) do not need this,
            // since they already are drawing a mouse cursor at every frame
            if( mouse().cursor ) {
                float scale = 1.0f;
                int shape = igMouseLastCursor();
                ImGuiViewport *vp = igGetMainViewport();
                ImU32 color_fill = ~0u, color_border = 0, color_shadow = 0;
                igRenderMouseCursor(ImVec2(vp->WorkPos.x+mouse().x,vp->WorkPos.y+mouse().y),scale,shape,color_fill,color_border,color_shadow);
            }
        }
        // due to async nature of pixel data, we have to skip a few initial frames in the recording.
        // if we dont do this, output video thumbnail could be black.
        if( wait_frames++ > 3 )
            record_frame();
    } else {
        wait_frames = 0;
    }
}

AUTORUN {
    hooks("tick", record_tick);
}

#endif
