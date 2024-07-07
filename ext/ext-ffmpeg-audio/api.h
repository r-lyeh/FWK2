#if CODE

AUTORUN {

    if( is_folder("ext/ext-ffmpeg") ) // ffmpeg required
    recipe(
        // paths and extensions. expected errorlevel
        "**.flac;**.mp3;", 0,
        // conversion steps
        "ext/ext-ffmpeg/ffmpeg.EXE -hide_banner -nostdin -loglevel fatal -y -i INPUT -threads 1 -y -c:a libvorbis -b:a 64k OUTPUT.ogg " /* -ac 1 -b:a 128k -ar 44100 */ " && REN OUTPUT.ogg OUTPUT\n"
    );

    if(!is_folder("ext/ext-audio-mod2wav") ) // modules may use mod2wav recipes instead, if present
    if( is_folder("ext/ext-ffmpeg") )  // ffmpeg required
    recipe(
        // paths and extensions. expected errorlevel
        "**.mod;**.xm;**.s3m;**.it", 0,
        // conversion steps
        "ext/ext-ffmpeg/ffmpeg.EXE -hide_banner -nostdin -loglevel fatal -y -i INPUT -threads 1 -y -c:a libvorbis -b:a 64k OUTPUT.ogg " /* -ac 1 -b:a 128k -ar 44100 */ " && REN OUTPUT.ogg OUTPUT\n"
    );
}

#endif
