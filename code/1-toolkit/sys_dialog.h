API void        alert(const char *message);
API void        alert2(const char *caption, const char *message);
API char*       prompt(const char *title, const char *caption, const char *defaults );

API const char* dialog_load();
API const char* dialog_save();

// 0 [ok], 1 [yes/no], 2 [yes/no/cancel]
API int         dialog(const char *caption, const char *message, int choices);

// ------------------------------------------------------------------------

#if CODE

int dialog(const char *caption, const char *message, int numbuttons) {
    const SDL_MessageBoxButtonData buttons[] = {
        // 1-button dialog
        { /* .flags, .buttonid, .text */        0, 0, "Ok" },
        // 2-button dialog
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No" },
        { /* .flags, .buttonid, .text */        0, 0, "Yes" },
        // 3-button dialog
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Cancel" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "No" },
        { /* .flags, .buttonid, .text */        0, 0, "Yes" },
    };
    SDL_MessageBoxData mb = {0};
    mb.flags = strstri(caption, "error") ? SDL_MESSAGEBOX_ERROR :
        strstri(caption, "warning") ? SDL_MESSAGEBOX_WARNING : SDL_MESSAGEBOX_INFORMATION;
    mb.title = caption;
    mb.message = message;
    mb.numbuttons = numbuttons = max(min(numbuttons,3),1);
    mb.buttons = buttons + (numbuttons == 3 ? 3 : numbuttons == 2 ? 1 : 0);

    int clicked;
    if( SDL_ShowMessageBox(&mb, &clicked) )
        return clicked+1;

    return 0;
}

#if is(osx)

char* prompt(const char *title, const char *body, const char *defaults ) {
    static char buffer[256]; buffer[0] = '\0';
    char *cmd = va("osascript -e 'text returned of (display dialog \"%s - %s\" default answer \"%s\")'", title, body, defaults);
    for( FILE *fp = popen(cmd, "r"); fp; pclose(fp), fp = 0 ) {
        fgets(buffer, 256, fp);
    }
    puts(buffer);
    return buffer;
}

#elif is(linux)

char* prompt(const char *title, const char *body, const char *defaults ) {
    // order should be: kdialog, then zenity, then Xdialog
    // kdialog --title "title" --inputbox "" "body"
    // zenity --title "title" --entry --text "body"
    // Xdialog
    static char buffer[256]; buffer[0] = '\0';
    char *cmdk = va("kdialog --title \"%s\" --inputbox \"%s\" \"%s\"", title, body, defaults);
    char *cmdz = va("zenity --title \"%s\" --entry --text \"%s\" --entry-text \"%s\"", title, body, defaults);
    for( FILE *fp = popen(va("%s || %s", cmdk, cmdz), "r"); fp; pclose(fp), fp = 0 ) {
        fgets(buffer, 256, fp);
    }
    puts(buffer);
    return buffer;
}

#endif


void alert2(const char *title, const char *body) {
    SDL_ShowSimpleMessageBox(0, title, body, NULL);
}

void alert(const char *message) {
    alert2("Warning", message);
}

const char* dialog_load() {
    // Not using this methods for now, as they're async:
    // SDL_ShowOpenFileDialog
    // SDL_ShowOpenFolderDialog

    const char *windowTitle = NULL;
    const char *defaultPathFile = NULL;
    const char *filterHints = NULL; // "image files"
    const char *filters[] = { "*.*" };
    int allowMultipleSelections = 0;

    tinyfd_assumeGraphicDisplay = 1;
    return tinyfd_openFileDialog( windowTitle, defaultPathFile, COUNTOF(filters), filters, filterHints, allowMultipleSelections );
}
const char* dialog_save() {
    // Not using this methods for now, as they're async:
    // SDL_ShowSaveFileDialog
    
    const char *windowTitle = NULL;
    const char *defaultPathFile = NULL;
    const char *filterHints = NULL; // "image files"
    const char *filters[] = { "*.*" };

    tinyfd_assumeGraphicDisplay = 1;
    return tinyfd_saveFileDialog( windowTitle, defaultPathFile, COUNTOF(filters), filters, filterHints );
}

#endif
