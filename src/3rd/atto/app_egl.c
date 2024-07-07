#include <EGL/egl.h>

// clang-format off
static const EGLint a__app_egl_config_attrs[] = {
	EGL_RED_SIZE, 5,
	EGL_GREEN_SIZE, 6,
	EGL_BLUE_SIZE, 5,
	EGL_DEPTH_SIZE, 16,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_NONE
};

static const EGLint a__app_egl_context_attrs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};
// clang-format on

// Can be referenced from outside by using extern
EGLDisplay a_app_egl_display;

static struct a__EglContext {
	EGLContext context;
	EGLSurface surface;
} a__app_egl;

static void a__appEglInit(EGLNativeDisplayType native_display, EGLNativeWindowType native_window) {
	EGLint ver_min, ver_maj;
	EGLConfig config;
	EGLint num_config;

	ATTO_ASSERT(EGL_NO_DISPLAY != (a_app_egl_display = eglGetDisplay(native_display)));
	ATTO_ASSERT(eglInitialize(a_app_egl_display, &ver_maj, &ver_min));

	aAppDebugPrintf("EGL: version %d.%d", ver_maj, ver_min);
	aAppDebugPrintf("EGL: EGL_VERSION: '%s'", eglQueryString(a_app_egl_display, EGL_VERSION));
	aAppDebugPrintf("EGL: EGL_VENDOR: '%s'", eglQueryString(a_app_egl_display, EGL_VENDOR));
	aAppDebugPrintf("EGL: EGL_CLIENT_APIS: '%s'", eglQueryString(a_app_egl_display, EGL_CLIENT_APIS));
	aAppDebugPrintf("EGL: EGL_EXTENSIONS: '%s'", eglQueryString(a_app_egl_display, EGL_EXTENSIONS));

	ATTO_ASSERT(eglChooseConfig(a_app_egl_display, a__app_egl_config_attrs, &config, 1, &num_config));

	a__app_egl.context = eglCreateContext(a_app_egl_display, config, EGL_NO_CONTEXT, a__app_egl_context_attrs);
	ATTO_ASSERT(EGL_NO_CONTEXT != a__app_egl.context);

	a__app_egl.surface = eglCreateWindowSurface(a_app_egl_display, config, native_window, 0);
	ATTO_ASSERT(EGL_NO_SURFACE != a__app_egl.surface);

	ATTO_ASSERT(eglMakeCurrent(a_app_egl_display, a__app_egl.surface, a__app_egl.surface, a__app_egl.context));
}

static void a__appEglSwap(void) {
	ATTO_ASSERT(eglSwapBuffers(a_app_egl_display, a__app_egl.surface));
}

static void a__appEglDeinit(void) {
	ATTO_ASSERT(a_app_egl_display != EGL_NO_DISPLAY);
	eglTerminate(a_app_egl_display);
}
