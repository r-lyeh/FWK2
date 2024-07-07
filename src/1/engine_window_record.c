
int window_record(const char *outfile_mp4) {
    record_start(outfile_mp4);
    // @todo: if( flags & RECORD_MOUSE )
    if( record_active() ) window_cursor_shape(CURSOR_SW_AUTO); else window_cursor_shape(CURSOR_HW_ARROW);
    return record_active();
}
