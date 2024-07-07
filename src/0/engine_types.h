#define vec2i(x, y     )  CAST(vec2i,(int)(x),   (int)(y)                          )
#define vec3i(x, y, z  )  CAST(vec3i,(int)(x),   (int)(y),   (int)(z)              )
#define vec2(x, y      )  CAST(vec2, (float)(x), (float)(y)                        )
#define vec3(x, y, z   )  CAST(vec3, (float)(x), (float)(y), (float)(z),           )
#define vec4(x, y, z, w)  CAST(vec4, (float)(x), (float)(y), (float)(z), (float)(w))
#define quat(x, y, z, w)  CAST(quat, (float)(x), (float)(y), (float)(z), (float)(w))
#define axis(x, y, z)     CAST(axis, (float)(x), (float)(y), (float)(z))
#define mat33(...)        CAST(mat33, __VA_ARGS__ )
#define mat34(...)        CAST(mat34, __VA_ARGS__ )
#define mat44(...)        CAST(mat44, __VA_ARGS__ )

typedef union vec2i{ struct { int X,Y; };       struct { int x,y; }; struct { int r,g; }; struct { int w,h; }; struct { int min,max; }; struct { int from,to; }; struct { int src,dst; }; int v2[2]; int array[1]; } vec2i;
typedef union vec3i{ struct { int X,Y,Z; };     struct { int x,y,z; }; struct { int r,g,b; }; struct { int w,h,d; }; struct { int min,max; }; struct { int from,to,step; }; struct { int src,dst; }; int v3[3]; int array[1]; } vec3i;
typedef union vec2 { struct { float X,Y; };     struct { float x,y; }; struct { float r,g; }; struct { float w,h; }; struct { float min,max; }; struct { float from,to; }; struct { float src,dst; }; float v2[2]; float array[1]; } vec2;
typedef union vec3 { struct { float X,Y,Z; };   struct { float x,y,z; }; struct { float r,g,b; }; struct { float min,max; }; struct { float from,to; }; vec2 xy; vec2 rg; vec2 wh; float v3[3]; float array[1]; } vec3;
typedef union vec4 { struct { float X,Y,Z,W; }; struct { float x,y,z,w; }; struct { float r,g,b,a; }; struct { float min,max; }; struct { float from,to; }; vec2 xy; vec3 xyz; vec2 rg; vec3 rgb; vec2 wh; vec3 whd; float v4[4]; float array[1]; } vec4;
typedef union quat { struct { float X,Y,Z,W; }; struct { float x,y,z,w; }; vec3 xyz; vec4 xyzw; float v4[4]; float array[1]; } quat;
typedef float mat33[9];
typedef float mat34[12];
typedef float mat44[16];
