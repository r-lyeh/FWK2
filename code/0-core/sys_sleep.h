#ifdef _WIN32
#define sys_sleep(ms) timerSleep((ms)/1000.0) // SleepEx((ms), FALSE)
#define sys_yield()   SwitchToThread()
#else
#define sys_sleep(ms) usleep((ms)*1000)
#define sys_yield()   usleep(0)
#endif


