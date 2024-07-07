#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------------------------
// Opaque Types
//--------------------------------------------------------------------------------------------------
typedef struct CCSG_World           CCSG_World;
typedef struct CCSG_Brush           CCSG_Brush;
typedef struct CCSG_VolumeOperation CCSG_VolumeOperation;

typedef struct CCSG_BrushSet          CCSG_BrushSet;
typedef struct CCSG_BrushSet_Iterator CCSG_BrushSet_Iterator;

typedef struct CCSG_BrushVec    CCSG_BrushVec;
typedef struct CCSG_RayHitVec   CCSG_RayHitVec;
typedef struct CCSG_FaceVec     CCSG_FaceVec;
typedef struct CCSG_PlaneVec    CCSG_PlaneVec;
typedef struct CCSG_TriangleVec CCSG_TriangleVec;

//--------------------------------------------------------------------------------------------------
// Reinterpreted Types - Must maintain these in sync with csg types
//--------------------------------------------------------------------------------------------------
typedef int CCSG_Volume;
typedef float CCSG_Vec3[3];
typedef float CCSG_Mat4[16];

typedef struct CCSG_Plane {
    CCSG_Vec3 normal;
    float offset;
} CCSG_Plane;

typedef struct CCSG_Face {
    const CCSG_Plane *plane;
    const void* _private_0[3];
    const void* _private_1[3];
} CCSG_Face;

typedef struct CCSG_Fragment {
    CCSG_Face *face;
    const void* _private_0[3];
    CCSG_Volume front_volume;
    CCSG_Volume back_volume;
    CCSG_Brush *front_brush;
    CCSG_Brush *back_brush;
    int _private_1;
} CCSG_Fragment;

typedef struct CCSG_Ray {
    CCSG_Vec3 origin, direction;
} CCSG_Ray;

typedef struct CCSG_RayHit {
    CCSG_Brush *brush;
    CCSG_Face *face;
    CCSG_Fragment *fragment;
    float parameter;
    CCSG_Vec3 position;
} CCSG_RayHit;

typedef struct CCSG_Box {
    CCSG_Vec3 min, max;
} CCSG_Box;

typedef struct CCSG_Vertex {
    CCSG_Vec3 position;
    const void* _private_0[3];
} CCSG_Vertex;

typedef struct CCSG_Triangle {
    int i, j, k;
} CCSG_Triangle;

//--------------------------------------------------------------------------------------------------
// Memory
//--------------------------------------------------------------------------------------------------
typedef void *(*CCSG_AllocateFunction)(size_t in_size); // Must be 16 byte aligned
typedef void (*CCSG_FreeFunction)(void *in_block);
typedef void *(*CCSG_AlignedAllocateFunction)(size_t in_size, size_t in_alignment);
typedef void (*CCSG_AlignedFreeFunction)(void *in_block);

void
CCSG_RegisterCustomAllocator(CCSG_AllocateFunction in_alloc,
                             CCSG_FreeFunction in_free,
                             CCSG_AlignedAllocateFunction in_aligned_alloc,
                             CCSG_AlignedFreeFunction in_aligned_free);

//--------------------------------------------------------------------------------------------------
// STL Container Methods
//--------------------------------------------------------------------------------------------------
void
CCSG_BrushSet_Destroy(CCSG_BrushSet *set);

CCSG_BrushSet_Iterator*
CCSG_BrushSet_Iterator_Begin(CCSG_BrushSet *set);

int // returns 0 if iterator has reached the end of the set
CCSG_BrushSet_Iterator_Next(CCSG_BrushSet *set, CCSG_BrushSet_Iterator *last);

const CCSG_Brush*
CCSG_BrushSet_Iterator_Read(const CCSG_BrushSet_Iterator *iterator);

void
CCSG_BrushSet_Iterator_Destroy(CCSG_BrushSet_Iterator *iterator);

//--------------------------------------------------------------------------------------------------
void
CCSG_BrushVec_Destroy(CCSG_BrushVec *vec);

size_t // Return value is length of array
CCSG_BrushVec_GetPtr(const CCSG_BrushVec *vec, const CCSG_Brush *const **out_array);

//--------------------------------------------------------------------------------------------------
void
CCSG_RayHitVec_Destroy(CCSG_RayHitVec *vec);

size_t // Return value is length of array
CCSG_RayHitVec_GetPtr(const CCSG_RayHitVec *vec, const CCSG_RayHit **out_array);

//--------------------------------------------------------------------------------------------------
void
CCSG_TriangleVec_Destroy(CCSG_TriangleVec *vec);

size_t // Return value is length of array
CCSG_TriangleVec_GetPtr(const CCSG_TriangleVec *vec, const CCSG_Triangle **out_array);

//--------------------------------------------------------------------------------------------------
// Volume Operations
//--------------------------------------------------------------------------------------------------
CCSG_VolumeOperation*
CCSG_MakeFillOperation(CCSG_Volume volume);

CCSG_VolumeOperation*
CCSG_MakeConvertOperation(CCSG_Volume from, CCSG_Volume to);

void
CCSG_VolumeOperation_Destroy(CCSG_VolumeOperation *operation);

//--------------------------------------------------------------------------------------------------
// CCSG_World
//--------------------------------------------------------------------------------------------------
CCSG_World*
CCSG_World_Create();

void
CCSG_World_Destroy(CCSG_World *world);

CCSG_Brush*
CCSG_World_First(CCSG_World *world);

CCSG_Brush*
CCSG_World_Next(CCSG_World *world, CCSG_Brush *brush);

void
CCSG_World_Remove(CCSG_World *world, CCSG_Brush *brush);

CCSG_Brush*
CCSG_World_Add(CCSG_World *world);

CCSG_BrushSet* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_Rebuild(CCSG_World *world);

void
CCSG_World_SetVoidVolume(CCSG_World *world, CCSG_Volume void_volume);

CCSG_Volume
CCSG_World_GetVoidVolume(const CCSG_World *world);

CCSG_BrushVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryPoint(CCSG_World *world, const CCSG_Vec3 *point);

CCSG_BrushVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryBox(CCSG_World *world, const CCSG_Box *box);

CCSG_RayHitVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryRay(CCSG_World *world, const CCSG_Ray *ray);

CCSG_BrushVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryFrustum(CCSG_World *world, const CCSG_Mat4 *view_projection);

void*
CCSG_World_GetUserData(const CCSG_World *world);

void
CCSG_World_SetUserData(CCSG_World *world, void *user_data);

//--------------------------------------------------------------------------------------------------
// CCSG_Brush
//--------------------------------------------------------------------------------------------------
void // Makes a copy of the caller's owned memory. The caller's array can be freed afterward.
CCSG_Brush_SetPlanes(CCSG_Brush *brush, const CCSG_Plane *plane_array, size_t array_length);

const CCSG_PlaneVec* // Returns pointer to library-owned memory. The caller may not manipulate nor free it.
CCSG_Brush_GetPlanes(const CCSG_Brush *brush);

void
CCSG_Brush_SetVolumeOperation(CCSG_Brush *brush, const CCSG_VolumeOperation *operation);

const CCSG_FaceVec* // Returns pointer to library-owned memory. The caller may not manipulate nor free it.
CCSG_Brush_GetFaces(const CCSG_Brush *brush);

void
CCSG_Brush_SetTime(CCSG_Brush *brush, int time);

int
CCSG_Brush_GetTime(const CCSG_Brush *brush);

int
CCSG_Brush_GetUid(const CCSG_Brush *brush);

CCSG_Box
CCSG_Brush_GetBox(const CCSG_Brush *brush);

void*
CCSG_Brush_GetUserData(const CCSG_Brush *brush);

void
CCSG_Brush_SetUserData(CCSG_Brush *brush, void *user_data);

//--------------------------------------------------------------------------------------------------
// CCSG_Fragment
//--------------------------------------------------------------------------------------------------
size_t // Return value is length of array
CCSG_Fragment_GetVerticesPtr(const CCSG_Fragment *fragment, const CCSG_Vertex **out_array);

//--------------------------------------------------------------------------------------------------
// CCSG_Face
//--------------------------------------------------------------------------------------------------
size_t // Return value is length of array
CCSG_Face_GetVerticesPtr(const CCSG_Face *face, const CCSG_Vertex **out_array);

size_t // Return value is length of array
CCSG_Face_GetFragmentsPtr(const CCSG_Face *face, const CCSG_Fragment **out_array);

size_t // Return value is length of array
CCSG_FaceVec_GetPtr(const CCSG_FaceVec *vec, const CCSG_Face **outArray);

//--------------------------------------------------------------------------------------------------
// Misc.
//--------------------------------------------------------------------------------------------------
CCSG_TriangleVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_Triangulate(const CCSG_Fragment *fragment);

size_t // Return value is length of array
CCSG_PlaneVec_GetPtr(const CCSG_PlaneVec *vec, const CCSG_Plane **outArray);

//--------------------------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif