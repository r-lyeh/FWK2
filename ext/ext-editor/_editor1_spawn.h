// ----------------------------------------------------------------------------------------
// editor instancing

API void editor_addtoworld(obj *o);
API void editor_watch(const void *o);
API void* editor_spawn(const char *ini); // deprecate?
API void editor_spawn1();

API void editor_destroy_selected();
API void editor_inspect(obj *o);

// ----------------------------------------------------------------------------------------

void editor_addtoworld(obj *o) {
    set_find_or_add(editor.world, o);
    for each_objchild(o, obj*, oo) {
        editor_addtoworld(oo);
    }
}

void editor_watch(const void *o) {
    array_push(editor.objs, (obj*)o);
    obj_push(o); // save state

    editor_addtoworld((obj*)o);
}
void* editor_spawn(const char *ini) { // deprecate?
    obj *o = obj_make(ini);
    editor_watch(o);
    return o;
}
void editor_spawn1() {
    obj *selected = editor_first_selected();
    obj *o = selected ? obj_make(obj_saveini(selected)) : obj_new(obj);
    if( selected ) obj_attach(selected, o), editor_setopen(selected, 1);
    else
    editor_watch(o);

    editor_unselect();
    editor_setselected(o, 1);
}

typedef set(obj*) set_objp_t;
static
void editor_glob_recurse(set_objp_t*list, obj *o) {
    set_find_or_add(*list, o);
    for each_objchild(o,obj*,oo) {
        editor_glob_recurse(list, oo);
    }
}
void editor_destroy_selected() {
    set_objp_t list = 0;
    set_init_ptr(list);
    for each_map_ptr(*editor_selected_map(), obj*,o, int,selected) {
        if( *selected ) { editor_glob_recurse(&list, *o); }
    }
    for each_set(list, obj*, o) {
        obj_detach(o);
    }
    for each_set(list, obj*, o) {
        // printf("deleting %p %s\n", o, obj_name(o));
        // remove from watched items
        for (int i = 0, end = array_count(editor.objs); i < end; ++i) {
            if (editor.objs[i] == o) {
                editor.objs[i] = 0;
                array_erase_slow(editor.objs, i);
                --end;
                --i;
            }
        }
        // delete from world
        set_erase(editor.world, o);
        // delete properties + obj
        editor_destroy_properties(o);
        obj_free(o);
    }
    set_free(list);
}
