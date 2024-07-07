
// ----------------------------------------------------------------------------------------
// editor selection

API void editor_select(const char *mask);
API void editor_unselect(); // same than editor_select("!**");

API void editor_select_aabb(aabb box);
API void editor_selectgroup(obj *first, obj *last);
API void* editor_first_selected();
API void* editor_last_selected();

// ----------------------------------------------------------------------------------------

    // all of them (hierarchical)
    array(obj*) objs; // @todo:set() world?
    // all of them (flat)
    set(obj*) world;


static
int editor_select_(void *o, const char *mask) {
    int matches = 0;
    int off = mask[0] == '!', inv = mask[0] == '~';
    int match = strmatchi(obj_type(o), mask+off+inv) || strmatchi(obj_name(o), mask+off+inv);
    if( match ) {
        editor_setselected(o, inv ? editor_selected(o) ^ 1 : !off);
        ++matches;
    }
    for each_objchild(o, obj*, oo) {
        matches += editor_select_(oo, mask);
    }
    return matches;
}
void editor_select(const char *mask) {
    for each_array( editor.objs, obj*, o )
        editor_select_(o, mask);
}
void editor_unselect() { // same than editor_select("!**");
    for each_map_ptr(*editor_selected_map(), void*,o, int, k) {
        if( *k ) *k = 0;
    }
}

void editor_select_aabb(aabb box) {
    int is_inv = input_held(KEY_CTRL);
    int is_add = input_held(KEY_SHIFT);
    if( !is_inv && !is_add ) editor_unselect();

    aabb item = {0};
    for each_set_ptr( editor.world, obj*, o ) {
        if( obj_hasmethod(*o,aabb) && obj_aabb(*o, &item) ) {
            if( aabb_test_aabb(item, box) ) {
                if( is_inv )
                editor_altselected(*o);
                else
                editor_setselected(*o, 1);
            }
        }
    }
}

static obj* active_ = 0;
static void editor_selectgroup_(obj *o, obj *first, obj *last) {
    // printf("%s (looking for %s in [%s..%s])\n", obj_name(o), active_ ? obj_name(active_) : "", obj_name(first), obj_name(last));
    if( !active_ ) if( o == first || o == last ) active_ = o == first ? last : first;
    if( active_ ) editor_setselected(o, 1);
    if( o == active_ ) active_ = 0;
    for each_objchild(o, obj*, oo) {
        editor_selectgroup_(oo, first, last);
    }
}
void editor_selectgroup(obj *first, obj *last) {
    if( last ) {
        if( !first ) first = array_count(editor.objs) ? editor.objs[0] : NULL;
        if( !first ) editor_setselected(last, 1);
        else {
            active_ = 0;
            for each_array(editor.objs,obj*,o) {
                editor_selectgroup_(o, first, last);
            }
        }
    }
}

static obj *find_any_selected_(obj *o) {
    if( editor_selected(o) ) return o;
    for each_objchild(o,obj*,oo) {
        obj *ooo = find_any_selected_(oo);
        if( ooo )
            return ooo;
    }
    return 0;
}
void* editor_first_selected() {
    for each_array(editor.objs,obj*,o) {
        obj *oo = find_any_selected_(o);
        // if( oo ) printf("1st found: %s\n", obj_name(oo));
        if( oo ) return oo;
    }
    return 0;
}

static obj *find_last_selected_(obj *o) {
    void *last = 0;
    if( editor_selected(o) ) last = o;
    for each_objchild(o,obj*,oo) {
        obj *ooo = find_last_selected_(oo);
        if( ooo )
            last = ooo;
    }
    return last;
}
void* editor_last_selected() {
    void *last = 0;
    for each_array(editor.objs,obj*,o) {
        obj *oo = find_last_selected_(o);
        // if( oo ) printf("last found: %s\n", obj_name(oo));
        if( oo ) last = oo;
    }
    return last;
}
