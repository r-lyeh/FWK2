#include <assert.h>
#include <stddef.h>
#include "ccsg.h"

//--------------------------------------------------------------------------------------------------
// Custom Allocator
//--------------------------------------------------------------------------------------------------
#ifdef CSG_CUSTOM_ALLOCATOR_HEADER
#include "ccsg_memory.hpp"
CCSG::AllocateFunction CCSG::Allocate = nullptr;
CCSG::FreeFunction CCSG::Free = nullptr;
CCSG::AlignedAllocateFunction CCSG::AlignedAllocate = nullptr;
CCSG::AlignedFreeFunction CCSG::AlignedFree = nullptr;
void
CCSG_RegisterCustomAllocator(CCSG_AllocateFunction in_alloc,
                             CCSG_FreeFunction in_free,
                             CCSG_AlignedAllocateFunction in_aligned_alloc,
                             CCSG_AlignedFreeFunction in_aligned_free)
{
    CCSG::Allocate = in_alloc;
    CCSG::Free = in_free;
    CCSG::AlignedAllocate = in_aligned_alloc;
    CCSG::AlignedFree = in_aligned_free;
}
#else // CSG_CUSTOM_ALLOCATOR_HEADER
void
CCSG_RegisterCustomAllocator(CCSG_AllocateFunction in_alloc,
                             CCSG_FreeFunction in_free,
                             CCSG_AlignedAllocateFunction in_aligned_alloc,
                             CCSG_AlignedFreeFunction in_aligned_free)
{ assert(!"Must define CSG_CUSTOM_ALLOCATOR_HEADER to register custom allocator!"); }
#endif // CSG_CUSTOM_ALLOCATOR_HEADER

#include "csg.hpp"

//--------------------------------------------------------------------------------------------------
// Safety Checks
//--------------------------------------------------------------------------------------------------
#define SIZE_ASSERT(C_TYPE, CPP_TYPE) static_assert(sizeof(C_TYPE) == sizeof(CPP_TYPE));
#define LAYOUT_ASSERTS(C_TYPE, CPP_TYPE, C_MEMBER, CPP_MEMBER) \
    static_assert(sizeof(C_TYPE) == sizeof(CPP_TYPE)); \
    static_assert(offsetof(C_TYPE, C_MEMBER) == offsetof(CPP_TYPE, CPP_MEMBER));

static_assert(sizeof(glm::vec3) == 3 * sizeof(float));
static_assert(sizeof(glm::mat4) == 16 * sizeof(float));
static_assert(sizeof(csg::vector_t<csg::brush_t*>) == 3 * sizeof(const void*));

SIZE_ASSERT(CCSG_Vec3, glm::vec3)
SIZE_ASSERT(CCSG_Mat4, glm::mat4)
SIZE_ASSERT(const void*[3], csg::vector_t<csg::plane_t>)

LAYOUT_ASSERTS(CCSG_Plane, csg::plane_t, offset, offset)
LAYOUT_ASSERTS(CCSG_Ray, csg::ray_t, direction, direction)
LAYOUT_ASSERTS(CCSG_RayHit, csg::ray_hit_t, position, position)
LAYOUT_ASSERTS(CCSG_Box, csg::box_t, max, max)
LAYOUT_ASSERTS(CCSG_Vertex, csg::vertex_t, _private_0, faces)
LAYOUT_ASSERTS(CCSG_Triangle, csg::triangle_t, k, k)

LAYOUT_ASSERTS(CCSG_Fragment, csg::fragment_t, back_brush, back_brush)
LAYOUT_ASSERTS(CCSG_Face, csg::face_t, _private_1, fragments)
#undef LAYOUT_ASSERTS
#undef SIZE_ASSERT

//--------------------------------------------------------------------------------------------------
// STL Container Aliases
//--------------------------------------------------------------------------------------------------
using VolumeOperation = csg::volume_operation_t;

using BrushSet = csg::set_t<csg::brush_t*>;
using BrushSet_Iterator = BrushSet::iterator;

using BrushVec = csg::vector_t<csg::brush_t*>;
using RayHitVec = csg::vector_t<csg::ray_hit_t>;
using FaceVec = csg::vector_t<csg::face_t>;
using PlaneVec = csg::vector_t<csg::plane_t>;
using TriangleVec = csg::vector_t<csg::triangle_t>;

//--------------------------------------------------------------------------------------------------
// C <---> C++ Pointer Cast Helpers
//--------------------------------------------------------------------------------------------------
#define C_CPP_PTR_CONVERT(C_TYPE, CPP_TYPE) \
    static auto toC(const CPP_TYPE *in) { assert(in); return reinterpret_cast<const C_TYPE *>(in); } \
    static auto toCpp(const C_TYPE *in) { assert(in); return reinterpret_cast<const CPP_TYPE *>(in); } \
    static auto toC(CPP_TYPE *in) { assert(in); return reinterpret_cast<C_TYPE *>(in); } \
    static auto toCpp(C_TYPE *in) { assert(in); return reinterpret_cast<CPP_TYPE *>(in); } \

C_CPP_PTR_CONVERT(CCSG_World, csg::world_t)
C_CPP_PTR_CONVERT(CCSG_Face, csg::face_t)
C_CPP_PTR_CONVERT(CCSG_Brush, csg::brush_t)
C_CPP_PTR_CONVERT(CCSG_Brush*, csg::brush_t*)
C_CPP_PTR_CONVERT(CCSG_Brush *const, csg::brush_t *const)
C_CPP_PTR_CONVERT(CCSG_Fragment, csg::fragment_t)

C_CPP_PTR_CONVERT(CCSG_Plane, csg::plane_t)
C_CPP_PTR_CONVERT(CCSG_Ray, csg::ray_t)
C_CPP_PTR_CONVERT(CCSG_RayHit, csg::ray_hit_t)
C_CPP_PTR_CONVERT(CCSG_Box, csg::box_t)
C_CPP_PTR_CONVERT(CCSG_Vertex, csg::vertex_t)
C_CPP_PTR_CONVERT(CCSG_Triangle, csg::triangle_t)

C_CPP_PTR_CONVERT(CCSG_VolumeOperation, VolumeOperation);

C_CPP_PTR_CONVERT(CCSG_BrushSet, BrushSet)
C_CPP_PTR_CONVERT(CCSG_BrushSet_Iterator, BrushSet_Iterator)

C_CPP_PTR_CONVERT(CCSG_BrushVec, BrushVec)
C_CPP_PTR_CONVERT(CCSG_RayHitVec, RayHitVec)
C_CPP_PTR_CONVERT(CCSG_FaceVec, FaceVec)
C_CPP_PTR_CONVERT(CCSG_PlaneVec, PlaneVec)
C_CPP_PTR_CONVERT(CCSG_TriangleVec, TriangleVec)

C_CPP_PTR_CONVERT(CCSG_Vec3, glm::vec3)
C_CPP_PTR_CONVERT(CCSG_Mat4, glm::mat4)

#undef C_CPP_PTR_CONVERT

//--------------------------------------------------------------------------------------------------
// STL Container Methods
//--------------------------------------------------------------------------------------------------
void
CCSG_BrushSet_Destroy(CCSG_BrushSet *set) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        toCpp(set)->~BrushSet();
        CCSG::Free(toCpp(set));
#   else
        delete toCpp(set);
#   endif
}

CCSG_BrushSet_Iterator*
CCSG_BrushSet_Iterator_Begin(CCSG_BrushSet *set) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto iterator = static_cast<BrushSet_Iterator*>(CCSG::Allocate(sizeof(BrushSet_Iterator)));
            ::new (iterator) BrushSet_Iterator(toCpp(set)->begin());
#   else
        auto iterator = new BrushSet_Iterator(toCpp(set)->begin());
#   endif
    return toC(iterator);
}

int // returns 0 if iterator has reached the end of the set
CCSG_BrushSet_Iterator_Next(CCSG_BrushSet *set, CCSG_BrushSet_Iterator *last) {
    auto iterator = *(toCpp(last));
    if (iterator == toCpp(set)->end()) return 0;
    std::advance(*(toCpp(last)), 1);
    return 1;
}

const CCSG_Brush*
CCSG_BrushSet_Iterator_Read(const CCSG_BrushSet_Iterator *iterator) { return toC(**(toCpp(iterator))); }

void
CCSG_BrushSet_Iterator_Destroy(CCSG_BrushSet_Iterator *iterator) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        toCpp(iterator)->~BrushSet_Iterator();
        CCSG::Free(toCpp(iterator));
#   else
        delete toCpp(iterator);
#   endif
}

void
CCSG_BrushVec_Destroy(CCSG_BrushVec *vec) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        toCpp(vec)->~BrushVec();
        CCSG::Free(toCpp(vec));
#   else
        delete toCpp(vec);
#   endif
}

size_t // Return value is length of array
CCSG_BrushVec_GetPtr(const CCSG_BrushVec *vec, const CCSG_Brush *const **out_array) {
    if (toCpp(vec)->empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(vec)->data());
    return toCpp(vec)->size();
}

void
CCSG_RayHitVec_Destroy(CCSG_RayHitVec *vec) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        toCpp(vec)->~RayHitVec();
        CCSG::Free(toCpp(vec));
#   else
        delete toCpp(vec);
#   endif
}

size_t // Return value is length of array
CCSG_RayHitVec_GetPtr(const CCSG_RayHitVec *vec, const CCSG_RayHit **out_array) {
    if (toCpp(vec)->empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(vec)->data());
    return toCpp(vec)->size();
}

void
CCSG_TriangleVec_Destroy(CCSG_TriangleVec *vec) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        toCpp(vec)->~TriangleVec();
        CCSG::Free(toCpp(vec));
#   else
        delete toCpp(vec);
#   endif
}

size_t // Return value is length of array
CCSG_TriangleVec_GetPtr(const CCSG_TriangleVec *vec, const CCSG_Triangle **out_array) {
    if (toCpp(vec)->empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(vec)->data());
    return toCpp(vec)->size();
}

//--------------------------------------------------------------------------------------------------
// Volume Operations
//--------------------------------------------------------------------------------------------------
CCSG_VolumeOperation*
CCSG_MakeFillOperation(CCSG_Volume volume) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto op = static_cast<VolumeOperation*>(CCSG::Allocate(sizeof(VolumeOperation)));
            ::new (op) VolumeOperation(csg::make_fill_operation(volume));
#   else
        auto op = new VolumeOperation(csg::make_fill_operation(volume));
#   endif
    return toC(op);
}

CCSG_VolumeOperation*
CCSG_MakeConvertOperation(CCSG_Volume from, CCSG_Volume to) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto op = static_cast<VolumeOperation*>(CCSG::Allocate(sizeof(VolumeOperation)));
            ::new (op) VolumeOperation(csg::make_convert_operation(from, to));
#   else
        auto op = new VolumeOperation(csg::make_convert_operation(from, to));
#   endif
    return toC(op);
}

void
CCSG_VolumeOperation_Destroy(CCSG_VolumeOperation *operation) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        toCpp(operation)->~VolumeOperation();
        CCSG::Free(toCpp(operation));
#   else
        delete toCpp(operation);
#   endif
}

//--------------------------------------------------------------------------------------------------
// CCSG_World
//--------------------------------------------------------------------------------------------------
CCSG_World*
CCSG_World_Create() {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        assert(CCSG::Allocate && "Must register custom allocator first if CSG_CUSTOM_ALLOCATOR_HEADER is defined");
#   endif
    return toC(new csg::world_t());
}

void
CCSG_World_Destroy(CCSG_World *world) { delete toCpp(world); }

CCSG_Brush*
CCSG_World_First(CCSG_World *world) {
    auto first = toCpp(world)->first();
    if (!first) return nullptr;
    return toC(first);
}

CCSG_Brush*
CCSG_World_Next(CCSG_World *world, CCSG_Brush *brush) {
    auto next = toCpp(world)->next(toCpp(brush));
    if (!next) return nullptr;
    return toC(next);
}

void
CCSG_World_Remove(CCSG_World *world, CCSG_Brush *brush) { toCpp(world)->remove(toCpp(brush)); }

CCSG_Brush*
CCSG_World_Add(CCSG_World *world) { return toC(toCpp(world)->add()); }

CCSG_BrushSet* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_Rebuild(CCSG_World *world) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto brush_set = static_cast<BrushSet*>(CCSG::Allocate(sizeof(BrushSet)));
            ::new (brush_set) BrushSet(toCpp(world)->rebuild());
#   else
        auto brush_set = new BrushSet(toCpp(world)->rebuild());
#   endif
    return toC(brush_set);
}

void
CCSG_World_SetVoidVolume(CCSG_World *world, CCSG_Volume void_volume) { toCpp(world)->set_void_volume(void_volume); }

CCSG_Volume
CCSG_World_GetVoidVolume(const CCSG_World *world) { return toCpp(world)->get_void_volume(); }

CCSG_BrushVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryPoint(CCSG_World *world, const CCSG_Vec3 *point) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto brush_vec = static_cast<BrushVec*>(CCSG::Allocate(sizeof(BrushVec)));
            ::new (brush_vec) BrushVec(toCpp(world)->query_point(*toCpp(point)));
#   else
        auto brush_vec = new BrushVec(toCpp(world)->query_point(*toCpp(point)));
#   endif
    return toC(brush_vec);
}

CCSG_BrushVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryBox(CCSG_World *world, const CCSG_Box *box) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto brush_vec = static_cast<BrushVec*>(CCSG::Allocate(sizeof(BrushVec)));
            ::new (brush_vec) BrushVec(toCpp(world)->query_box(*toCpp(box)));
#   else
        auto brush_vec = new BrushVec(toCpp(world)->query_box(*toCpp(box)));
#   endif
    return toC(brush_vec);
}

CCSG_RayHitVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryRay(CCSG_World *world, const CCSG_Ray *ray) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto ray_hit_vec = static_cast<RayHitVec*>(CCSG::Allocate(sizeof(RayHitVec)));
            ::new (ray_hit_vec) RayHitVec(toCpp(world)->query_ray(*toCpp(ray)));
#   else
        auto ray_hit_vec = new RayHitVec(toCpp(world)->query_ray(*toCpp(ray)));
#   endif
    return toC(ray_hit_vec);
}

CCSG_BrushVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_World_QueryFrustum(CCSG_World *world, const CCSG_Mat4 *view_projection) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto brush_vec = static_cast<BrushVec*>(CCSG::Allocate(sizeof(BrushVec)));
            ::new (brush_vec) BrushVec(toCpp(world)->query_frustum(*toCpp(view_projection)));
#   else
        auto brush_vec = new BrushVec(toCpp(world)->query_frustum(*toCpp(view_projection)));
#   endif
    return toC(brush_vec);
}

void*
CCSG_World_GetUserData(const CCSG_World *world) { return std::any_cast<void*>(toCpp(world)->userdata); }

void
CCSG_World_SetUserData(CCSG_World *world, void *user_data) { toCpp(world)->userdata = std::make_any<void*>(user_data); }

//--------------------------------------------------------------------------------------------------
// CCSG_Brush
//--------------------------------------------------------------------------------------------------
void // Makes a copy of the caller's owned memory. The caller's array can be freed afterward.
CCSG_Brush_SetPlanes(CCSG_Brush *brush, const CCSG_Plane *plane_array, size_t array_length) {
    PlaneVec copied_vec(toCpp(plane_array), toCpp(plane_array + array_length));
    toCpp(brush)->set_planes(copied_vec);
}

const CCSG_PlaneVec* // Returns pointer to library-owned memory. The caller may not manipulate nor free it.
CCSG_Brush_GetPlanes(const CCSG_Brush *brush) { return toC(&toCpp(brush)->get_planes()); }

void
CCSG_Brush_SetVolumeOperation(CCSG_Brush *brush, const CCSG_VolumeOperation *operation) {
    toCpp(brush)->set_volume_operation(*toCpp(operation));
}

const CCSG_FaceVec* // Returns pointer to library-owned memory. The caller may not manipulate nor free it.
CCSG_Brush_GetFaces(const CCSG_Brush *brush) { return toC(&toCpp(brush)->get_faces()); }

void
CCSG_Brush_SetTime(CCSG_Brush *brush, int time) { toCpp(brush)->set_time(time); }

int
CCSG_Brush_GetTime(const CCSG_Brush *brush) { return toCpp(brush)->get_time(); }

int
CCSG_Brush_GetUid(const CCSG_Brush *brush) { return toCpp(brush)->get_uid(); }

CCSG_Box
CCSG_Brush_GetBox(const CCSG_Brush *brush) {
    auto box = toCpp(brush)->get_box();
    return CCSG_Box {
        { box.min.x, box.min.y, box.min.z },
        { box.max.x, box.max.y, box.max.z },
    };
}

void*
CCSG_Brush_GetUserData(const CCSG_Brush *brush) { return std::any_cast<void*>(toCpp(brush)->userdata); }

void
CCSG_Brush_SetUserData(CCSG_Brush *brush, void *user_data) { toCpp(brush)->userdata = std::make_any<void*>(user_data); }

//--------------------------------------------------------------------------------------------------
// CCSG_Fragment
//--------------------------------------------------------------------------------------------------
size_t // Return value is length of array
CCSG_Fragment_GetVerticesPtr(const CCSG_Fragment *fragment, const CCSG_Vertex **out_array) {
    if (toCpp(fragment)->vertices.empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(fragment)->vertices.data());
    return toCpp(fragment)->vertices.size();
}

//--------------------------------------------------------------------------------------------------
// CCSG_Face
//--------------------------------------------------------------------------------------------------
size_t // Return value is length of array
CCSG_Face_GetVerticesPtr(const CCSG_Face *face, const CCSG_Vertex **out_array) {
    if (toCpp(face)->vertices.empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(face)->vertices.data());
    return toCpp(face)->vertices.size();
}

size_t // Return value is length of array
CCSG_Face_GetFragmentsPtr(const CCSG_Face *face, const CCSG_Fragment **out_array) {
    if (toCpp(face)->fragments.empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(face)->fragments.data());
    return toCpp(face)->fragments.size();
}

size_t
CCSG_FaceVec_GetPtr(const CCSG_FaceVec *vec, const CCSG_Face **out_array) {
    if (toCpp(vec)->empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(vec)->data());
    return toCpp(vec)->size();
}

//--------------------------------------------------------------------------------------------------
// Misc.
//--------------------------------------------------------------------------------------------------
CCSG_TriangleVec* // Returns a pointer to copied memory that will be owned and freed by the caller.
CCSG_Triangulate(const CCSG_Fragment *fragment) {
#   ifdef CSG_CUSTOM_ALLOCATOR_HEADER
        auto triangle_vec = static_cast<TriangleVec*>(CCSG::Allocate(sizeof(TriangleVec)));
            ::new (triangle_vec) TriangleVec(csg::triangulate(*toCpp(fragment)));
#   else
        auto triangle_vec = new TriangleVec(csg::triangulate(*toCpp(fragment)));
#   endif
    return toC(triangle_vec);
}

size_t
CCSG_PlaneVec_GetPtr(const CCSG_PlaneVec *vec, const CCSG_Plane **out_array) {
    if (toCpp(vec)->empty()) {
        (*out_array) = nullptr;
        return 0;
    }
    (*out_array) = toC(toCpp(vec)->data());
    return toCpp(vec)->size();
}