#ifndef ATTO_PLATFORM
#define ATTO_PLATFORM

#ifdef __linux__
	#define ATTO_PLATFORM_POSIX
	#define ATTO_PLATFORM_LINUX
	#ifndef ATTO_PLATFORM_RPI
		#define ATTO_PLATFORM_X11
	#else
		#define ATTO_PLATFORM_EGL
		#define ATTO_PLATFORM_EVDEV
	#endif /* ifndef ATTO_PLATFORM_RPI */
#elif defined(_WIN32)
	#define ATTO_PLATFORM_WINDOWS
#elif defined(__MACH__) && defined(__APPLE__)
	#define ATTO_PLATFORM_POSIX
	#define ATTO_PLATFORM_MACH
	#define ATTO_PLATFORM_OSX
#else
	#error Not ported
#endif

#endif /* ifndef ATTO_PLATFORM */
