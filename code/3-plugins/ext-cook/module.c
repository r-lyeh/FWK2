#include "engine_cook.c"

void init2() {
#if HAS_COOK
        // create or update cook.zip file
        if( /* !COOK_ON_DEMAND && */ have_tools() && cook_jobs() ) {
            cook_start(COOK_INI, "**", 0|COOK_ASYNC|COOK_CANCELABLE );
        }
#endif
}
void quit2() {

}
void precook2() {
}

void postcook2() {
#if HAS_COOK
    // cook cleanup
    cook_stop();
#endif

    // clean any errno setup by cooking stage
    errno = 0;
}

void cook2() {
    static float previous[JOBS_MAX] = {0};

    #define ddraw_progress_bar(JOB_ID, JOB_MAX, PERCENT) do { \
       /* NDC coordinates (2d): bottom-left(-1,-1), center(0,0), top-right(+1,+1) */ \
       float progress = (PERCENT+1) / 100.f; if(progress > 1) progress = 1; \
       float speed = progress < 1 ? 0.05f : 0.75f; \
       float smooth = previous[JOB_ID] = progress * speed + previous[JOB_ID] * (1-speed); \
       \
       float pixel = 2.f / window_height(), dist = smooth*2-1, y = pixel*3*JOB_ID; \
       if(JOB_ID==0)ddraw_line(vec3(-1,y-pixel*2,0), vec3(1,   y-pixel*2,0)); /* full line */ \
       ddraw_line(vec3(-1,y-pixel  ,0), vec3(dist,y-pixel  ,0)); /* progress line */ \
       ddraw_line(vec3(-1,y+0      ,0), vec3(dist,y+0      ,0)); /* progress line */ \
       ddraw_line(vec3(-1,y+pixel  ,0), vec3(dist,y+pixel  ,0)); /* progress line */ \
       if(JOB_ID==JOB_MAX-1)ddraw_line(vec3(-1,y+pixel*2,0), vec3(1,   y+pixel*2,0)); /* full line */ \
    } while(0)

    // if( FLAGS_TRANSPARENT ) {} else // @transparent
    for(int i = 0; i < cook_jobs(); ++i) ddraw_progress_bar(i, cook_jobs(), jobs[i].progress);
    // ddraw_progress_bar(0, 1, cook_progress());

    ddraw_flush();
}
