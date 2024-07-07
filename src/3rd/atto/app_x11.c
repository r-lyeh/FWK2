#ifndef ATTO_APP_WIDTH
	#define ATTO_APP_WIDTH 1280
#endif

#ifndef ATTO_APP_HEIGHT
	#define ATTO_APP_HEIGHT 720
#endif

#ifndef ATTO_APP_NAME
	#define ATTO_APP_NAME "atto app"
#endif

#include "atto/platform.h"
#include "atto/app.h"

#define GL_GLEXT_PROTOTYPES 1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xfixes.h>
#ifndef ATTO_EGL
#include <GL/glx.h>
#else
#include <EGL/egl.h>
#endif

#include <string.h>
#include <stdlib.h> /* exit() */

static struct AAppState a__app_state;
const struct AAppState *a_app_state = &a__app_state;

static struct AAppProctable a__app_proctable;

void aAppTerminate(int code) {
	exit(code);
}

static struct {
	Display *display;
	Window window;
#ifndef ATTO_EGL
	GLXDrawable drawable;
	GLXContext context;
#endif
} a__x11;

static void a__appProcessXKeyEvent(XEvent *e) {
	ATimeUs timestamp = aAppTime();
	AKey key = AK_Unknown;
	int down = KeyPress == e->type;
	switch (XLookupKeysym(&e->xkey, 0)) {
#define ATTOMAPK__(x, a) \
	case XK_##x: key = AK_##a; break;
		ATTOMAPK__(BackSpace, Backspace)
		ATTOMAPK__(Tab, Tab)
		ATTOMAPK__(Return, Enter)
		ATTOMAPK__(space, Space)
		ATTOMAPK__(Escape, Esc)
		ATTOMAPK__(Page_Up, PageUp)
		ATTOMAPK__(Page_Down, PageDown)
		ATTOMAPK__(Left, Left)
		ATTOMAPK__(Up, Up)
		ATTOMAPK__(Right, Right)
		ATTOMAPK__(Down, Down)
		ATTOMAPK__(comma, Comma)
		ATTOMAPK__(minus, Minus)
		ATTOMAPK__(period, Dot)
		ATTOMAPK__(slash, Slash)
		ATTOMAPK__(equal, Equal)
		ATTOMAPK__(Delete, Del)
		ATTOMAPK__(Insert, Ins)
		ATTOMAPK__(Home, Home)
		ATTOMAPK__(End, End)
		ATTOMAPK__(asterisk, KeypadAsterisk)
		ATTOMAPK__(0, 0)
		ATTOMAPK__(1, 1)
		ATTOMAPK__(2, 2)
		ATTOMAPK__(3, 3)
		ATTOMAPK__(4, 4)
		ATTOMAPK__(5, 5)
		ATTOMAPK__(6, 6)
		ATTOMAPK__(7, 7)
		ATTOMAPK__(8, 8)
		ATTOMAPK__(9, 9)
		ATTOMAPK__(a, A)
		ATTOMAPK__(b, B)
		ATTOMAPK__(c, C)
		ATTOMAPK__(d, D)
		ATTOMAPK__(e, E)
		ATTOMAPK__(f, F)
		ATTOMAPK__(g, G)
		ATTOMAPK__(h, H)
		ATTOMAPK__(i, I)
		ATTOMAPK__(j, J)
		ATTOMAPK__(k, K)
		ATTOMAPK__(l, L)
		ATTOMAPK__(m, M)
		ATTOMAPK__(n, N)
		ATTOMAPK__(o, O)
		ATTOMAPK__(p, P)
		ATTOMAPK__(q, Q)
		ATTOMAPK__(r, R)
		ATTOMAPK__(s, S)
		ATTOMAPK__(t, T)
		ATTOMAPK__(u, U)
		ATTOMAPK__(v, V)
		ATTOMAPK__(w, W)
		ATTOMAPK__(x, X)
		ATTOMAPK__(y, Y)
		ATTOMAPK__(z, Z)
		ATTOMAPK__(KP_Add, KeypadPlus)
		ATTOMAPK__(KP_Subtract, KeypadMinus)
		ATTOMAPK__(F1, F1)
		ATTOMAPK__(F2, F2)
		ATTOMAPK__(F3, F3)
		ATTOMAPK__(F4, F4)
		ATTOMAPK__(F5, F5)
		ATTOMAPK__(F6, F6)
		ATTOMAPK__(F7, F7)
		ATTOMAPK__(F8, F8)
		ATTOMAPK__(F9, F9)
		ATTOMAPK__(F10, F10)
		ATTOMAPK__(F11, F11)
		ATTOMAPK__(F12, F12)
		ATTOMAPK__(Alt_L, LeftAlt)
		ATTOMAPK__(Control_L, LeftCtrl)
		ATTOMAPK__(Meta_L, LeftMeta)
		ATTOMAPK__(Super_L, LeftSuper)
		ATTOMAPK__(Shift_L, LeftShift)
		ATTOMAPK__(Alt_R, RightAlt)
		ATTOMAPK__(Control_R, RightCtrl)
		ATTOMAPK__(Meta_R, RightMeta)
		ATTOMAPK__(Super_R, RightSuper)
		ATTOMAPK__(Shift_R, RightShift)
		ATTOMAPK__(Caps_Lock, Capslock);
#undef ATTOMAPK_
	default: return;
	}

	if (a__app_state.keys[key] == down)
		return;

	a__app_state.keys[key] = down;

	if (a__app_proctable.key)
		a__app_proctable.key(timestamp, key, down);
}

static void a__appProcessXButton(const XEvent *e) {
	unsigned int button = 0;
	ATimeUs timestamp = aAppTime();
	int dx, dy;
	unsigned int buttons_changed_bits;
	const unsigned int pressed = e->xbutton.type == ButtonPress;

	switch (e->xbutton.button) {
	case Button1: button = AB_Left; break;
	case Button2: button = AB_Middle; break;
	case Button3: button = AB_Right; break;
	case Button4: button = AB_WheelUp; break;
	case Button5: button = AB_WheelDown; break;
	}

	if (pressed)
		buttons_changed_bits = a__app_state.pointer.buttons ^ button;
	else
		buttons_changed_bits = a__app_state.pointer.buttons & button;

	a__app_state.pointer.buttons ^= buttons_changed_bits;

	dx = e->xbutton.x - a__app_state.pointer.x;
	dy = e->xbutton.y - a__app_state.pointer.y;
	a__app_state.pointer.x = e->xbutton.x;
	a__app_state.pointer.y = e->xbutton.y;

	if (a__app_proctable.pointer)
		a__app_proctable.pointer(timestamp, dx, dy, buttons_changed_bits);
}

static void a__appProcessXMotion(const XEvent *e) {
	ATimeUs timestamp = aAppTime();
	int dx = e->xmotion.x - a__app_state.pointer.x, dy = e->xmotion.y - a__app_state.pointer.y;

	a__app_state.pointer.x = e->xmotion.x;
	a__app_state.pointer.y = e->xmotion.y;

	if (a__app_state.grabbed) {
		if (e->xmotion.x == (int)a__app_state.width / 2 && e->xmotion.y == (int)a__app_state.height / 2)
			return;

		XWarpPointer(a__x11.display, None, a__x11.window, 0, 0, 0, 0, a__app_state.width / 2, a__app_state.height / 2);
	}

	if (a__app_proctable.pointer)
		a__app_proctable.pointer(timestamp, dx, dy, 0);
}

#ifndef ATTO_EGL
static const int a__glxattribs[] = {
	GLX_X_RENDERABLE, True,
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE, GLX_RGBA_BIT,
	GLX_CONFIG_CAVEAT, GLX_NONE,
	GLX_RED_SIZE, 8,
	GLX_GREEN_SIZE, 8,
	GLX_BLUE_SIZE, 8,
	GLX_ALPHA_SIZE, 8,
	GLX_DEPTH_SIZE, 24,
	GLX_DOUBLEBUFFER, True,
	0
};
#else
static struct a__EglContext {
	EGLContext context;
	EGLSurface surface;
} a__app_egl;

static const EGLint a__app_egl_config_attrs[] = {
	EGL_BUFFER_SIZE, 32,
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,

	EGL_DEPTH_SIZE, 24,
	EGL_STENCIL_SIZE, EGL_DONT_CARE,

	EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,

	EGL_NONE
};

static const EGLint a__app_egl_context_attrs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};

// Can be referenced from outside by using extern
EGLDisplay a_app_egl_display;
#endif

int main(int argc, char *argv[]) {
	ATimeUs timestamp = aAppTime();
	XSetWindowAttributes winattrs;
	Atom delete_message;
#ifndef ATTO_EGL
	int nglxconfigs = 0;
	GLXFBConfig *glxconfigs = NULL;
#endif
	XVisualInfo *vinfo = NULL;
	ATimeUs last_paint = 0;

	ATTO_ASSERT(a__x11.display = XOpenDisplay(NULL));

#ifndef ATTO_EGL
	ATTO_ASSERT(glxconfigs = glXChooseFBConfig(a__x11.display, 0, a__glxattribs, &nglxconfigs));
	ATTO_ASSERT(nglxconfigs);

	ATTO_ASSERT(vinfo = glXGetVisualFromFBConfig(a__x11.display, glxconfigs[0]));

#else
	EGLint ver_min, ver_maj;
	EGLConfig config;
	EGLint num_config;

	eglBindAPI(EGL_OPENGL_API);

	ATTO_ASSERT(EGL_NO_DISPLAY != (a_app_egl_display = eglGetDisplay(a__x11.display)));
	ATTO_ASSERT(eglInitialize(a_app_egl_display, &ver_maj, &ver_min));

	aAppDebugPrintf("EGL: version %d.%d", ver_maj, ver_min);
	aAppDebugPrintf("EGL: EGL_VERSION: '%s'",
		eglQueryString(a_app_egl_display, EGL_VERSION));
	aAppDebugPrintf("EGL: EGL_VENDOR: '%s'",
		eglQueryString(a_app_egl_display, EGL_VENDOR));
	aAppDebugPrintf("EGL: EGL_CLIENT_APIS: '%s'",
		eglQueryString(a_app_egl_display, EGL_CLIENT_APIS));
	aAppDebugPrintf("EGL: EGL_EXTENSIONS: '%s'",
		eglQueryString(a_app_egl_display, EGL_EXTENSIONS));

	ATTO_ASSERT(eglChooseConfig(a_app_egl_display, a__app_egl_config_attrs,
		&config, 1, &num_config));

	ATTO_ASSERT(num_config > 0);

	{
		XVisualInfo xvisual_info = {0};
		int num_visuals;
		ATTO_ASSERT(eglGetConfigAttrib(a_app_egl_display, config, EGL_NATIVE_VISUAL_ID, (EGLint*)&xvisual_info.visualid));
		//xvisual_info.screen = DefaultScreen(a__x11.display);

		ATTO_ASSERT(vinfo = XGetVisualInfo(a__x11.display, VisualScreenMask | VisualIDMask, &xvisual_info, &num_visuals));
	}
#endif // ATTO_EGL

	memset(&winattrs, 0, sizeof(winattrs));
	winattrs.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		ExposureMask | VisibilityChangeMask | StructureNotifyMask;
	winattrs.border_pixel = 0;
	winattrs.bit_gravity = StaticGravity;
	winattrs.colormap =
		XCreateColormap(a__x11.display, RootWindow(a__x11.display, vinfo->screen), vinfo->visual, AllocNone);
	winattrs.override_redirect = False;

	a__x11.window =
		XCreateWindow(a__x11.display, RootWindow(a__x11.display, vinfo->screen), 0, 0, ATTO_APP_WIDTH, ATTO_APP_HEIGHT, 0,
			vinfo->depth, InputOutput, vinfo->visual, CWBorderPixel | CWBitGravity | CWEventMask | CWColormap, &winattrs);
	ATTO_ASSERT(a__x11.window);

	XStoreName(a__x11.display, a__x11.window, ATTO_APP_NAME);

	delete_message = XInternAtom(a__x11.display, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(a__x11.display, a__x11.window, &delete_message, 1);

	XMapWindow(a__x11.display, a__x11.window);

	XkbSetDetectableAutoRepeat(a__x11.display, True, NULL);

#ifndef ATTO_EGL
	ATTO_ASSERT(a__x11.context = glXCreateNewContext(a__x11.display, glxconfigs[0], GLX_RGBA_TYPE, 0, True));
	ATTO_ASSERT(a__x11.drawable = glXCreateWindow(a__x11.display, glxconfigs[0], a__x11.window, 0));

	glXMakeContextCurrent(a__x11.display, a__x11.drawable, a__x11.drawable, a__x11.context);
#else
	a__app_egl.context = eglCreateContext(a_app_egl_display, config,
		EGL_NO_CONTEXT, a__app_egl_context_attrs);
	ATTO_ASSERT(EGL_NO_CONTEXT != a__app_egl.context);

	a__app_egl.surface = eglCreateWindowSurface(a_app_egl_display, config, a__x11.window, 0);
	//a__app_egl.surface = eglCreatePlatformWindowSurface(a_app_egl_display, config, &native_window, 0);
	ATTO_ASSERT(EGL_NO_SURFACE != a__app_egl.surface);

	ATTO_ASSERT(eglMakeCurrent(a_app_egl_display, a__app_egl.surface,
		a__app_egl.surface, a__app_egl.context));
#endif // !ATTO_EGL

	XSelectInput(a__x11.display, a__x11.window,
		StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
	a__app_state.argc = argc;
	a__app_state.argv = (const char *const *)argv;
	a__app_state.gl_version = AOGLV_21;
	a__app_state.width = ATTO_APP_WIDTH;
	a__app_state.height = ATTO_APP_HEIGHT;

	ATTO_APP_INIT_FUNC(&a__app_proctable);

	if (a__app_proctable.resize)
		a__app_proctable.resize(timestamp, 0, 0);

	for (;;) {
		while (XPending(a__x11.display)) {
			XEvent e;
			XNextEvent(a__x11.display, &e);
			switch (e.type) {
			case ConfigureNotify: {
				unsigned int oldw = a__app_state.width, oldh = a__app_state.height;

				if (a__app_state.width == (unsigned int)e.xconfigure.width &&
					a__app_state.height == (unsigned int)e.xconfigure.height)
					break;

				a__app_state.width = e.xconfigure.width;
				a__app_state.height = e.xconfigure.height;
				timestamp = aAppTime();

				if (a__app_proctable.resize)
					a__app_proctable.resize(timestamp, oldw, oldh);
			} break;

			case ButtonPress:
			case ButtonRelease: a__appProcessXButton(&e); break;
			case MotionNotify: a__appProcessXMotion(&e); break;
			case KeyPress:
			case KeyRelease: a__appProcessXKeyEvent(&e); break;

			case ClientMessage:
			case DestroyNotify:
			case UnmapNotify: goto exit; break;
			}
		}

		{
			ATimeUs now = aAppTime();
			float dt;
			if (!last_paint)
				last_paint = now;
			dt = (now - last_paint) * 1e-6f;

			if (a__app_proctable.paint)
				a__app_proctable.paint(now, dt);

#ifndef ATTO_EGL
			glXSwapBuffers(a__x11.display, a__x11.drawable);
#else
			ATTO_ASSERT(eglSwapBuffers(a_app_egl_display, a__app_egl.surface));
#endif
			last_paint = now;
		}
	}

exit:
	if (a__app_proctable.close)
		a__app_proctable.close();

	aAppDebugPrintf("cleaning up");
#ifndef ATTO_EGL
	glXMakeContextCurrent(a__x11.display, 0, 0, 0);
	glXDestroyWindow(a__x11.display, a__x11.drawable);
	glXDestroyContext(a__x11.display, a__x11.context);
#else
	ATTO_ASSERT(a_app_egl_display != EGL_NO_DISPLAY);
	eglTerminate(a_app_egl_display);
#endif
	XDestroyWindow(a__x11.display, a__x11.window);
	XCloseDisplay(a__x11.display);

	return 0;
}

void aAppGrabInput(int grab) {
	if (grab == a_app_state->grabbed)
		return;
	if (grab) {
		XGrabPointer(
			a__x11.display, a__x11.window, True, 0, GrabModeAsync, GrabModeAsync, a__x11.window, None, CurrentTime);
		XFixesHideCursor(a__x11.display, a__x11.window);
		XWarpPointer(a__x11.display, None, a__x11.window, 0, 0, 0, 0, a__app_state.width / 2, a__app_state.height / 2);
	} else {
		XFixesShowCursor(a__x11.display, a__x11.window);
		XUngrabPointer(a__x11.display, CurrentTime);
	}
	a__app_state.grabbed = grab;
}
