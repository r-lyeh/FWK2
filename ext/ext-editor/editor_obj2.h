#if !CODE

typedef struct root { OBJ
} root;

API extern root* objroot;

OBJTYPEDEF(root,99);

// @fixme: move to obj_obj.h
API void obj_flagall(void *o, int flag, int mode);
API void obj_setup(void *o); // this is editor-only

typedef map(int,void*) map_id2obj_t;
typedef map(void*,int) map_obj2id_t;

API extern map_id2obj_t id2obj;
API extern map_obj2id_t obj2id;

#else

root* objroot;
void root_tick(root *o) {}
void root_draw(root *o) {}
AUTORUN {
    EXTEND(root, draw,tick);
    obj_setname(objroot = obj_new(root), "Game");
    obj_flag(objroot,|=,HAS_TICK|IS_TICKING|HAS_DRAW|IS_DRAWING|HAS_VIEW|HAS_CHILDREN|IS_OPEN);
}


map_id2obj_t id2obj;
map_obj2id_t obj2id;

void obj_setup(obj *o) { // @fixme: use objflags for these masks?
    if(!id2obj)    { map_init_int(id2obj);    map_insert(id2obj, 0, NULL); }
    if(!obj2id)    { map_init_ptr(obj2id);    map_insert(obj2id, NULL, 0); }
    int *found = map_find(obj2id, o);
    if( !found ) {
        int counter = o->objid; // static int counter = 0; ++counter;
        map_find_or_add(obj2id, o, counter );
        map_find_or_add(id2obj, counter, o );
    }

    for each_objchild(o,obj*,oo) {
        if( !obj_flag(oo,&,IS_COMPONENT) ) {
            obj_flag(o, |=, HAS_CHILDREN|IS_OPEN);
        }
    }

    if( obj_hasmethod(o, tick) ) obj_flag(o, |=, HAS_TICK|IS_TICKING);
    if( obj_hasmethod(o, draw) ) obj_flag(o, |=, HAS_DRAW|IS_DRAWING);
    if( obj_hasmethod(o, view) ) obj_flag(o, |=, HAS_VIEW);

    for each_objchild(o, obj*, child) {
        obj_setup(child);
    }
}

void obj_flagall(void *o, int flag, int mode) {
    /**/ if(mode == 1) obj_flag((obj*)o,|=,flag);
    else if(mode == 0) obj_flag((obj*)o,&=,~flag);
    else if(mode  < 0) obj_flag((obj*)o,^=,flag);
    for each_objchild(o, obj*, child)
        obj_flagall(child, flag, mode);
}

#endif
