#pragma once

#include <new>
#include <cstdio>

namespace CCSG {

    // must be at least 8 byte aligned on 32 bit platform and 16 byte aligned on 64 bit platform
    using AllocateFunction = void *(*)(size_t inSize);
    using FreeFunction = void (*)(void *inBlock);
    using AlignedAllocateFunction = void *(*)(size_t inSize, size_t inAlignment);
    using AlignedFreeFunction = void (*)(void *inBlock);
    
    // User defined allocation / free functions
    extern AllocateFunction Allocate;
    extern FreeFunction Free;
    extern AlignedAllocateFunction AlignedAllocate;
    extern AlignedFreeFunction AlignedFree;

    template<class T>
    struct STL_Allocator {
        typedef T value_type;

        STL_Allocator() = default;

        template<class U>
        constexpr STL_Allocator(const STL_Allocator <U>&) noexcept {}

        [[nodiscard]] T* allocate(std::size_t n) noexcept {
            return static_cast<T*>(CCSG::Allocate(n * sizeof(T)));
        }
        void deallocate(T* p, std::size_t n) noexcept {
            CCSG::Free(p);
        }
    };

    template<class T, class U>
    bool operator==(const STL_Allocator <T>&, const STL_Allocator <U>&) { return true; }

    template<class T, class U>
    bool operator!=(const STL_Allocator <T>&, const STL_Allocator <U>&) { return false; }
}

#define CSG_CUSTOM_STL_ALLOCATOR(...) CCSG::STL_Allocator<__VA_ARGS__>

#define csg_vector(T) std::vector<T, CSG_CUSTOM_STL_ALLOCATOR(T)>
#define csg_set(T) std::set<T, std::less<T>, CSG_CUSTOM_STL_ALLOCATOR(T)>
#define csg_map(K, T) std::map<K, T, std::less<K>, CSG_CUSTOM_STL_ALLOCATOR(std::pair<const K, T>)>

#if defined(__clang__)
    #define CCSG_COMPILER_CLANG
#elif defined(__GNUC__)
    #define CCSG_COMPILER_GCC
#elif defined(_MSC_VER)
    #define CCSG_COMPILER_MSVC
#endif

#if defined(CCSG_COMPILER_CLANG) || defined(CCSG_COMPILER_GCC)
    #define CCSG_INLINE __inline__ __attribute__((always_inline))
#elif defined(CCSG_COMPILER_MSVC)
    #define CCSG_INLINE __forceinline
#else
    #error Undefined
#endif

#define csg_replace_new_delete \
    CCSG_INLINE void *operator new (size_t inCount) { return CCSG::Allocate(inCount); } \
    CCSG_INLINE void operator delete (void *inPointer) noexcept { CCSG::Free(inPointer); } \
    CCSG_INLINE void *operator new[] (size_t inCount) { return CCSG::Allocate(inCount); } \
    CCSG_INLINE void operator delete[] (void *inPointer) noexcept { CCSG::Free(inPointer); } \
    CCSG_INLINE void *operator new (size_t inCount, std::align_val_t inAlignment) \
        { return CCSG::AlignedAllocate(inCount, static_cast<size_t>(inAlignment)); } \
    CCSG_INLINE void operator delete (void *inPointer, std::align_val_t inAlignment) noexcept \
        { CCSG::AlignedFree(inPointer); } \
    CCSG_INLINE void *operator new[] (size_t inCount, std::align_val_t inAlignment) \
        { return CCSG::AlignedAllocate(inCount, static_cast<size_t>(inAlignment)); } \
    CCSG_INLINE void operator delete[] (void *inPointer, std::align_val_t inAlignment) noexcept \
        { CCSG::AlignedFree(inPointer); }

