// ----------------------------------------------------------------------------
// editor properties
// note: properties are not tied to objects. can be anything addressable (void*)

#if !CODE

#define EDITOR_PROPERTYDEF(T,property_name) \
    typedef map(void*,T) editor_##property_name##_map_t; \
API editor_##property_name##_map_t *editor_##property_name##_map(); \
API T editor_##property_name(const void *obj); \
API void editor_set##property_name(const void *obj, T value); \
API void editor_alt##property_name(const void *obj); \
API void editor_no##property_name(void *obj);

//EDITOR_PROPERTYDEF(int,  open);     ///- whether object is tree opened in tree editor
//EDITOR_PROPERTYDEF(int,  selected); ///- whether object is displaying a contextual popup or not
  EDITOR_PROPERTYDEF(int,  changed);  ///- whether object is displaying a contextual popup or not
//EDITOR_PROPERTYDEF(int,  popup);    ///- whether object is displaying a contextual popup or not
//EDITOR_PROPERTYDEF(int,  bookmarked); ///-
//EDITOR_PROPERTYDEF(int,  visible); ///-
//EDITOR_PROPERTYDEF(int,  script); ///-
//EDITOR_PROPERTYDEF(int,  event); ///-
//EDITOR_PROPERTYDEF(char*,iconinstance); ///-
//EDITOR_PROPERTYDEF(char*,iconclass); ///-
//EDITOR_PROPERTYDEF(int,  treeoffsety); ///-

API void editor_destroy_properties(void *o);

// ----------------------------------------------------------------------------
#else

typedef void (*editor_no_property)(void *);
array(void*) editor_persist_kv;
array(editor_no_property) editor_no_properties;

#define EDITOR_PROPERTY(T,property_name,defaults) \
editor_##property_name##_map_t *editor_##property_name##_map() { \
    static editor_##property_name##_map_t map = 0; do_once map_init_ptr(map); \
    return &map; \
} \
T editor_##property_name(const void *obj) { \
    return *map_find_or_add(*editor_##property_name##_map(), (void*)obj, ((T) defaults)); \
} \
void editor_set##property_name(const void *obj, T value) { \
    *map_find_or_add(*editor_##property_name##_map(), (void*)obj, ((T) value)) = ((T) value); \
} \
void editor_alt##property_name(const void *obj) { \
    T* found = map_find_or_add(*editor_##property_name##_map(), (void*)obj, ((T) defaults)); \
    *found = (T)(uintptr_t)!(*found); \
} \
void editor_no##property_name(void *obj) { \
    T* found = map_find_or_add(*editor_##property_name##_map(), (void*)obj, ((T) defaults)); \
    map_erase(*editor_##property_name##_map(), (void*)obj); \
} \
AUTORUN { array_push(editor_persist_kv, #T); array_push(editor_persist_kv, editor_##property_name##_map()); array_push(editor_no_properties, editor_no##property_name); }

void editor_destroy_properties(void *o) {
    for each_array(editor_no_properties,editor_no_property,fn) {
        fn(o);
    }
}

//EDITOR_PROPERTY(int,  open, 0);     ///-
//EDITOR_PROPERTY(int,  selected, 0); ///-
  EDITOR_PROPERTY(int,  changed, 0);  ///-
//EDITOR_PROPERTY(int,  popup, 0);    ///-
//EDITOR_PROPERTY(int,  bookmarked, 0); ///-
//EDITOR_PROPERTY(int,  visible, 0); ///-
//EDITOR_PROPERTY(int,  script, 0); ///-
//EDITOR_PROPERTY(int,  event, 0); ///-
//EDITOR_PROPERTY(char*,iconinstance, ""); ///-
//EDITOR_PROPERTY(char*,iconclass, ""); ///-
//EDITOR_PROPERTY(int,  treeoffsety, 0); ///-

#endif
