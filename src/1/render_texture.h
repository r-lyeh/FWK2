// -----------------------------------------------------------------------------
// colors

API unsigned rgba( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
API unsigned bgra( uint8_t b, uint8_t g, uint8_t r, uint8_t a );
API unsigned rgbaf( float r, float g, float b, float a );
API unsigned bgraf( float b, float g, float r, float a );
API unsigned alpha( unsigned rgba );

#define RGBX(rgb,x)   ( ((rgb)&0xFFFFFF) | (((unsigned)(x))<<24) )
#define RGB3(r,g,b)   ( (255<<24) | ((b)<<16) | ((g)<<8) | (r) )
#define RGB4(r,g,b,a) ( ((a)<<24) | ((b)<<16) | ((g)<<8) | (r) )

#define BLACK   RGBX(0x000000,255)
#define WHITE   RGBX(0xE8F1FF,255)

#define RED     RGB3(   255, 0,48 )
#define GREEN   RGB3(  144,255,48 )
#define CYAN    RGB3(   0,192,255 )
#define ORANGE  RGB3(  255,144,48 )
#define PURPLE  RGB3(  102,77,102 ) // 178,128,255 )
#define YELLOW  RGB3(   255,224,0 )
#define GRAY    RGB3(    32,32,32 ) // dark gray
#define SILVER  RGB3( 149,149,149 ) // dark white, gray-ish
#define PINK    RGB3(  255,48,144 )
#define AQUA    RGB3(  48,255,144 )

#define BLUE    RGBX(0xB55A06,255)

API unsigned atorgba(const char *s);
API char *   rgbatoa(unsigned rgba);

// -----------------------------------------------------------------------------
// images

/// flags when constructing the image_t type. see: image, image_from_mem
/// IMAGE_R: 1-channel image (R)
/// IMAGE_RG: 2-channel image (R,G)
/// IMAGE_RGB: 3-channel image (R,G,B)
/// IMAGE_RGBA: 4-channel image (R,G,B,A)
/// IMAGE_FLIP: Flip image vertically
/// IMAGE_FLOAT: Float pixel components
enum IMAGE_FLAGS {
    IMAGE_R     = 0x01000,
    IMAGE_RG    = 0x02000,
    IMAGE_RGB   = 0x04000,
    IMAGE_RGBA  = 0x08000,
    IMAGE_FLIP  = 0x10000,
    IMAGE_FLOAT = 0x20000,
};

/// type that holds linear uncompressed bitmap of any given dimensions.
/// w,h: image dimensions in pixels. `x,y` alias.
/// comps: number of components per pixel. `n` alias.
/// pixels: untyped pointer to linear bitmap data. typed pointers use `pixels8/16/32/f` aliases.
/// see: texture_t
typedef struct image_t {
    union { unsigned x, w; };
    union { unsigned y, h; };
    union { unsigned n, comps; };
    union { void *pixels; uint8_t *pixels8; uint16_t *pixels16; uint32_t *pixels32; float *pixelsf; };
} image_t;

API image_t image(const char *pathfile, int flags);
API image_t image_from_mem(const void *ptr, int len, int flags);
API void    image_destroy(image_t *img);

// -----------------------------------------------------------------------------
// textures

enum TEXTURE_FLAGS {
    // UNIT[0..7]

    TEXTURE_BC1 = 8,  // DXT1, RGB with 8:1 compression ratio (+ optional 1bpp for alpha)
    TEXTURE_BC2 = 16, // DXT3, RGBA with 4:1 compression ratio (BC1 for RGB + 4bpp for alpha)
    TEXTURE_BC3 = 32, // DXT5, RGBA with 4:1 compression ratio (BC1 for RGB + BC4 for A)
//  TEXTURE_BC4,     // Alpha

    TEXTURE_NEAREST = 0,
    TEXTURE_LINEAR = 64,
    TEXTURE_MIPMAPS = 128,
    TEXTURE_ANISOTROPY = 1 << 30,

    TEXTURE_CLAMP = 0,
    TEXTURE_BORDER = 0x100,
    TEXTURE_REPEAT = 0x200,

    TEXTURE_BYTE = 0,
    TEXTURE_FLOAT = IMAGE_FLOAT,

    TEXTURE_COLOR = 0,
    TEXTURE_DEPTH = 0x800,

    TEXTURE_R = IMAGE_R,
    TEXTURE_RG = IMAGE_RG,
    TEXTURE_RGB = IMAGE_RGB,
    TEXTURE_RGBA = IMAGE_RGBA,
    TEXTURE_FLIP = IMAGE_FLIP,

    // @fixme
    TEXTURE_SRGB = 1 << 24,
    TEXTURE_BGR = 1 << 25,
    TEXTURE_BGRA = TEXTURE_BGR,
    TEXTURE_ARRAY = 1 << 26,
};

typedef struct texture_t {
    union { unsigned x, w; };
    union { unsigned y, h; };
    union { unsigned z, d; };
    union { unsigned n, bpp; };
    handle id;
    unsigned texel_type;
    unsigned flags;
    char* filename;
    bool transparent;
    unsigned fbo; // for texture recording
    union { unsigned userdata, delay; };
} texture_t;

API texture_t texture_compressed(const char *filename, unsigned flags);
API texture_t texture_compressed_from_mem(const void *data, int len, unsigned flags);

API texture_t texture(const char* filename, int flags);
API texture_t texture_from_mem(const void* ptr, int len, int flags);
API texture_t texture_create(unsigned w, unsigned h, unsigned n, const void *pixels, int flags);
API texture_t texture_checker();
API void      texture_destroy(texture_t *t);
API int       texture_unit(); // returns rolling counter up to GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
// textureLod(filename, dir, lod);
// void texture_add_loader( int(*loader)(const char *filename, int *w, int *h, int *bpp, int reqbpp, int flags) );
API unsigned  texture_update(texture_t *t, unsigned w, unsigned h, unsigned n, const void *pixels, int flags);

API bool      texture_rec_begin(texture_t *t, unsigned w, unsigned h); // texture_rec
API void      texture_rec_end(texture_t *t); // texture_rec
