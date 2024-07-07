API array(obj*) editor_group_up(void *root, unsigned maskand, unsigned maskcmp);
API array(obj*) editor_group_down(void *root, unsigned maskand, unsigned maskcmp);

#if CODE

static
void _group_up(void *o, unsigned maskand, unsigned maskcmp, array(obj*) *group) {
    if( obj_flag(o,&,maskand) == maskcmp ) array_push(*group, o);
    if( (o = obj_parent(o)) != NULL ) {
        _group_up(o, maskand, maskcmp, group);
    }
}

array(obj*) editor_group_up(void *root, unsigned maskand, unsigned maskcmp) {
    static int slot; slot = (slot + 1) % 16;
    static array(obj*) group[16]; array_resize(group[slot], 0);
    _group_up(root, maskand, maskcmp, &group[slot]);
    return group[slot];
}

static
void _group_down(void *o, unsigned maskand, unsigned maskcmp, array(obj*) *group) {
    if( obj_flag(o,&,maskand) == maskcmp ) array_push(*group, o);
    for each_objchild(o, obj*, oo) {
        _group_down(oo, maskand, maskcmp, group);
    }
}

array(obj*) editor_group_down(void *root, unsigned maskand, unsigned maskcmp) {
    static int slot; slot = (slot + 1) % 16;
    static array(obj*) group[16]; array_resize(group[slot], 0);
    _group_down(root, maskand, maskcmp, &group[slot]);
    return group[slot];
}

#endif
