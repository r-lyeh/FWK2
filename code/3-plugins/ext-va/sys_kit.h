// -----------------------------------------------------------------------------
// ## localization kit (I18N, L10N)

API bool  kit_load( const char *filename ); // load translations file (xlsx)
API bool  kit_merge( const char *filename ); // merge translations file into existing context
API void  kit_insert( const char *id, const char *translation ); // insert single translation unit
API void  kit_clear(); // delete all translations

API void  kit_set( const char *variable, const char *value ); // set context variable
API void  kit_reset(); // reset all variables in context
API void  kit_dump_state( FILE *fp ); // debug

API char* kit_translate2( const char *id, const char *langcode_iso639_1 ); // perform a translation given explicit locale

API void  kit_locale( const char *langcode_iso639_1 ); // set current locale: enUS, ptBR, esES, ...
API char* kit_translate( const char *id ); // perform a translation, given current locale

// ----------------------------------------------------------------------------
// localization kit

static const char *kit_lang = "enUS", *kit_langs =
    "enUS,enGB,"
    "frFR,"
    "esES,esAR,esMX,"
    "deDE,deCH,deAT,"
    "itIT,itCH,"
    "ptBR,ptPT,"
    "zhCN,zhSG,zhTW,zhHK,zhMO,"
    "ruRU,elGR,trTR,daDK,noNB,svSE,nlNL,plPL,fiFI,jaJP,"
    "koKR,csCZ,huHU,roRO,thTH,bgBG,heIL"
;

static map(char*,char*) kit_ids;
static map(char*,char*) kit_vars;

#ifndef KIT_FMT_ID2
#define KIT_FMT_ID2 "%s.%s"
#endif

void kit_init() {
    do_once map_init(kit_ids, less_str, hash_str);
    do_once map_init(kit_vars, less_str, hash_str);
}

void kit_insert( const char *id, const char *translation) {
    char *id2 = va(KIT_FMT_ID2, kit_lang, id);

    char **found = map_find_or_add_allocated_key(kit_ids, STRDUP(id2), NULL);
    if(*found) FREE(*found);
    *found = STRDUP(translation);
}

bool kit_merge( const char *filename ) {
    // @todo: xlsx2ini
    return false;
}

void kit_clear() {
    map_clear(kit_ids);
}

bool kit_load( const char *filename ) {
    return kit_clear(), kit_merge( filename );
}

void kit_set( const char *key, const char *value ) {
    value = value && value[0] ? value : "";

    char **found = map_find_or_add_allocated_key(kit_vars, STRDUP(key), NULL );
    if(*found) FREE(*found);
    *found = STRDUP(value);
}

void kit_reset() {
    map_clear(kit_vars);
}

char *kit_translate2( const char *id, const char *lang ) {
    char *id2 = va(KIT_FMT_ID2, lang, id);

    char **found = map_find(kit_ids, id2);

    // return original [[ID]] if no translation is found
    if( !found ) return va("[[%s]]", id);

    // return translation if no {{moustaches}} are found
    if( !strstr(*found, "{{") ) return *found;

    // else replace all found {{moustaches}} with context vars
    {
        // make room
        static __thread char *results[16] = {0};
        static __thread unsigned counter = 0; counter = (counter+1) % 16;

        char *buffer = results[ counter ];
        if( buffer ) FREE(buffer), buffer = 0;

        // iterate moustaches
        const char *begin, *end, *text = *found;
        while( NULL != (begin = strstr(text, "{{")) ) {
            end = strstr(begin+2, "}}");
            if( end ) {
                char *var = va("%.*s", (int)(end - (begin+2)), begin+2);
                char **found_var = map_find(kit_vars, var);

                if( found_var && 0[*found_var] ) {
                    strcatf(&buffer, "%.*s%s", (int)(begin - text), text, *found_var);
                } else {
                    strcatf(&buffer, "%.*s{{%s}}", (int)(begin - text), text, var);
                }

                text = end+2;
            } else {
                strcatf(&buffer, "%.*s", (int)(begin - text), text);

                text = begin+2;
            }
        }

        strcatf(&buffer, "%s", text);
        return buffer;
    }
}

char *kit_translate( const char *id ) {
    return kit_translate2( id, kit_lang );
}

void kit_locale( const char *lang ) {
    kit_lang = STRDUP(lang); // @leak
}

void kit_dump_state( FILE *fp ) {
    for each_map(kit_ids, char *, k, char *, v) {
        fprintf(fp, "[ID ] %s=%s\n", k, v);
    }
    for each_map(kit_vars, char *, k, char *, v) {
        fprintf(fp, "[VAR] %s=%s\n", k, v);
    }
}

/*
int main() {
    kit_init();

    kit_locale("enUS");
    kit_insert("HELLO_PLAYERS", "Hi {{PLAYER1}} and {{PLAYER2}}!");
    kit_insert("GREET_PLAYERS", "Nice to meet you.");

    kit_locale("esES");
    kit_insert("HELLO_PLAYERS", "Hola {{PLAYER1}} y {{PLAYER2}}!");
    kit_insert("GREET_PLAYERS", "Un placer conoceros.");

    kit_locale("enUS");
    printf("%s %s\n", kit_translate("HELLO_PLAYERS"), kit_translate("GREET_PLAYERS")); // Hi {{PLAYER1}} and {{PLAYER2}}! Nice to meet you.

    kit_locale("esES");
    kit_set("PLAYER1", "John");
    kit_set("PLAYER2", "Karl");
    printf("%s %s\n", kit_translate("HELLO_PLAYERS"), kit_translate("GREET_PLAYERS")); // Hola John y Karl! Un placer conoceros.

    assert( 0 == strcmp(kit_translate("NON_EXISTING"), "[[NON_EXISTING]]")); // [[NON_EXISTING]]
    assert(~puts("Ok"));
}
*/
