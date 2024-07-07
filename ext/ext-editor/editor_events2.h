#if !CODE

API int editor_event_down(void *o, int mask, bool force_recurse, void* call, const char *fmt, ... );

#else

typedef void (*obj_call_t[256])();

typedef union event_arg_t {
    int i;
    double d;
//  const char *s;
} event_arg_t;

API int editor_event_down0(void *o, int mask, bool forced_recurse, obj_call_t call );
API int editor_event_down1(void *o, int mask, bool forced_recurse, obj_call_t call, event_arg_t argv[1] );
API int editor_event_down2(void *o, int mask, bool forced_recurse, obj_call_t call, event_arg_t argv[2] );

int editor_event_down0(void *o, int mask, bool forced, obj_call_t call) {
    if( obj_flag(o, &, mask) == mask ) obj_method2(call, o);
    if( obj_flag(o, &, mask) == mask || forced )
    for each_objchild(o, obj*, oo) {
        editor_event_down0(oo,mask,forced, call);
    }
    return 0;
}
int editor_event_down1i(void *o, int mask, bool forced, obj_call_t call, event_arg_t args[1]) {
    if( obj_flag(o, &, mask) == mask ) obj_method2(call, o, args[0].i);
    if( obj_flag(o, &, mask) == mask || forced )
    for each_objchild(o, obj*, oo) {
        editor_event_down1i(oo,mask,forced, call,args);
    }
    return 1;
}
int editor_event_down1d(void *o, int mask, bool forced, obj_call_t call, event_arg_t args[1]) {
    if( obj_flag(o, &, mask) == mask ) obj_method2(call, o, args[0].d);
    if( obj_flag(o, &, mask) == mask || forced )
    for each_objchild(o, obj*, oo) {
        editor_event_down1d(oo,mask,forced, call,args);
    }
    return 1;
}

int editor_event_down(void *o, int mask, bool forced_recurse, void *call, const char *fmt, ... ) {
    event_arg_t argv[1];

    int argc = 0;
    if( fmt ) {
        va_list va;
        va_start(va, fmt);
        for( ; fmt[argc]; ++argc ) {
            assert(argc < countof(argv));
            /**/ if( fmt[argc] == 'i' ) argv[argc].i = va_arg(va, int);
            else if( fmt[argc] == 'd' ) argv[argc].d = va_arg(va, double);
        //  else if( fmt[argc] == 's' ) argv[argc].s = va_arg(va, const char *);
            else alert2("!unknown event format", fmt);
        }
        va_end(va);
    }

    if( argc == 0 )                  return editor_event_down0 (o, mask, forced_recurse, call);
    if( argc == 1 && fmt[0] == 'i' ) return editor_event_down1i(o, mask, forced_recurse, call, argv);
    if( argc == 1 && fmt[0] == 'd' ) return editor_event_down1d(o, mask, forced_recurse, call, argv);
    return -1;
}

#endif
