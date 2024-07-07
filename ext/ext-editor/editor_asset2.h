#if !CODE

// our editor assets, composed of following chunks:
// [type] so we know what we're loading
// [cook] cooked contents. used by runtime. [cook] and [type] are the only required fields in a retail game
// [json] the options the editor used to cook the contents above. in json5 spec, so it's future bullet-proof.
// [data] the original file before any cooked was processed. useful when exporting or duplicating from editor.

typedef struct editor_asset_t {
    unsigned fourcc;          // asset type
    unsigned sizeof_cooked;   // cooked blob
    unsigned sizeof_imports;  // import settings blob
    unsigned sizeof_uncooked; // raw/uncooked blob
} editor_asset_t;

#else

#endif
