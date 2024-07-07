#if CODE

void editor_load_on_boot(void) {
}
void editor_save_on_quit(void) {
}
AUTORUN {
    editor_load_on_boot();
    (atexit)(editor_save_on_quit);
}

#endif
