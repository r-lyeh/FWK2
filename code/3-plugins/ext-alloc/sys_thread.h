API int         app_cores();

API void*       thread( int (*thread_func)(void* user_data), void* user_data );
API void        thread_destroy( void *thd );

// ----------------------------------------------------------------------------
// threads

struct thread_wrapper {
    int (*func)(void *user_data);
    void *user_data;
};

static
int thread_proc( void* user_data ) {
    struct thread_wrapper *w = (struct thread_wrapper*)user_data;
    int return_code = w->func( w->user_data );
    thread_exit( return_code );
    FREE(w);
    return 0;
}

void* thread( int (*thread_func)(void* user_data), void* user_data ) {
    struct thread_wrapper *w = MALLOC(sizeof(struct thread_wrapper));
    w->func = thread_func;
    w->user_data = user_data;

    int thread_stack_size = 0;
    const char *thread_name = "";
    thread_ptr_t thd = thread_init( thread_proc, w, thread_name, thread_stack_size );
    return thd;
}
void thread_destroy( void *thd ) {
    int rc = thread_join(thd);
    thread_term(thd);
}

// cpu -------------------------------------------------------------------------

#if is(linux)
#include <sched.h>
#endif

int app_cores() {
#if is(win32)
    DWORD_PTR pm, sm;
    if( GetProcessAffinityMask(GetCurrentProcess(), &pm, &sm) ) if( pm ) {
        int count = 0;
        while( pm ) {
            ++count;
            pm &= pm - 1;
        }
        return count;
    }
    { SYSTEM_INFO si; GetSystemInfo(&si); return (int)si.dwNumberOfProcessors; }
#else // unix
    int count = sysconf(_SC_NPROCESSORS_ONLN);
    return count > 0 ? count : 1;
#endif
#if 0
#elif is(linux)
    cpu_set_t prevmask, testmask;
    CPU_ZERO(&prevmask);
    CPU_ZERO(&testmask);
    sched_getaffinity(0, sizeof(prevmask), &prevmask);     //Get current mask
    sched_setaffinity(0, sizeof(testmask), &testmask);     //Set zero mask
    sched_getaffinity(0, sizeof(testmask), &testmask);     //Get mask for all CPUs
    sched_setaffinity(0, sizeof(prevmask), &prevmask);     //Reset current mask
    int num = CPU_COUNT(&testmask);
    return (num > 1 ? num : 1);
#elif is(cpp)
    return (int)std::thread::hardware_concurrency();
#elif defined(_OPENMP)
    // omp
    int cores = 0;
    #pragma omp parallel
    {
        #pragma omp atomic
        ++cores;
    }
    return cores;
#endif
}
