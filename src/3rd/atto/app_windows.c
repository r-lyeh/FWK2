#ifndef ATTO_APP_WIDTH
	#define ATTO_APP_WIDTH 1280
#endif

#ifndef ATTO_APP_HEIGHT
	#define ATTO_APP_HEIGHT 720
#endif

#ifndef ATTO_APP_NAME
	#define ATTO_APP_NAME "atto app"
#endif

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <windowsx.h> /* GET_X/Y_LPARAM */
#include <GL/gl.h>
/* #include "wglext.h"
#include <atto/platform.h> */
#include <atto/app.h>

/* static WCHAR *utf8_to_wchar(const char *string, int length, int *out_length); */
static char *wchar_to_utf8(const WCHAR *string, int length, int *out_length);
static void a__AppOpenConsole(void);
static void a__AppCleanup(void);
static AKey a__AppMapKey(WPARAM key);
static LRESULT CALLBACK a__AppWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

static struct AAppState a__app_state;
const struct AAppState *a_app_state = &a__app_state;
static struct AAppProctable a__app_proctable;

static const PIXELFORMATDESCRIPTOR a__pfd = {sizeof(a__pfd), 0,
	PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL, PFD_TYPE_RGBA, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0};

static LARGE_INTEGER a__time_freq;
static LARGE_INTEGER a__time_start;

static struct {
	HWND hwnd;
	HDC hdc;
	HGLRC hglrc;

	struct {
		int resetAbsolute;
		long x, y;
	} rawMouse;
} g;

ATimeUs aAppTime() {
	LARGE_INTEGER now;
	if (a__time_start.QuadPart == 0) {
		QueryPerformanceFrequency(&a__time_freq);
		QueryPerformanceCounter(&a__time_start);
	}
	QueryPerformanceCounter(&now);
	return (ATimeUs)((now.QuadPart - a__time_start.QuadPart) * 1000000ul / a__time_freq.QuadPart);
}

void aAppDebugPrintf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "DBG: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	fflush(stderr);
	va_end(args);
}

void aAppTerminate(int code) {
	a__AppCleanup();
	ExitProcess(code);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wndclass;
	ATimeUs last_paint = 0;

	(void)hPrevInstance;
	(void)lpCmdLine;

	a__AppOpenConsole();

	memset(&wndclass, 0, sizeof(wndclass));
	wndclass.cbSize = sizeof(wndclass);
	wndclass.lpszClassName = TEXT("atto");
	wndclass.lpfnWndProc = a__AppWndProc;
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	ATTO_ASSERT(RegisterClassEx(&wndclass));

	g.hwnd = CreateWindow(TEXT("atto"), TEXT(ATTO_APP_NAME), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0,
		0, ATTO_APP_WIDTH, ATTO_APP_HEIGHT, NULL, NULL, hInstance, NULL);
	ATTO_ASSERT(0 != g.hwnd);
	g.hdc = GetDC(g.hwnd);
	ATTO_ASSERT(0 != g.hdc);

	SetPixelFormat(g.hdc, ChoosePixelFormat(g.hdc, &a__pfd), &a__pfd);
	g.hglrc = wglCreateContext(g.hdc);
	ATTO_ASSERT(0 != g.hglrc);
	wglMakeCurrent(g.hdc, g.hglrc);

	{
		int i;
		char **argv;
		LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &a__app_state.argc);
		argv = malloc(sizeof(char *) * (a__app_state.argc + 1));
		for (i = 0; i < a__app_state.argc; ++i) argv[i] = wchar_to_utf8(wargv[i], -1, NULL);
		a__app_state.argv = (const char **)argv;
	}

	a__app_state.gl_version = AOGLV_21;
	a__app_state.width = ATTO_APP_WIDTH;
	a__app_state.height = ATTO_APP_HEIGHT;

	ATTO_APP_INIT_FUNC(&a__app_proctable);
	if (a__app_proctable.resize)
		a__app_proctable.resize(aAppTime(), 0, 0);

	ShowWindow(g.hwnd, nCmdShow);

	for (;;) {
		MSG msg;
		while (0 != PeekMessage(&msg, g.hwnd, 0, 0, PM_NOREMOVE)) {
			if (0 == GetMessage(&msg, NULL, 0, 0))
				goto exit;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		{
			ATimeUs now = aAppTime();
			if (!last_paint)
				last_paint = now;
			float dt = (now - last_paint) * 1e-6f;
			if (a__app_proctable.paint)
				a__app_proctable.paint(now, dt);
			SwapBuffers(g.hdc);
			last_paint = now;
		}
	}

exit:
	if (a__app_proctable.close)
		a__app_proctable.close();
	a__AppCleanup();
	return 0;
}

static void a__AppCleanup(void) {
	wglMakeCurrent(g.hdc, NULL);
	wglDeleteContext(g.hglrc);
	ReleaseDC(g.hwnd, g.hdc);
	DestroyWindow(g.hwnd);
}

static void a__AppOpenConsole(void) {
	AllocConsole();
	FILE *dummy_file;
	freopen_s(&dummy_file, "CONIN$", "r", stdin);
	freopen_s(&dummy_file, "CONOUT$", "w", stdout);
	freopen_s(&dummy_file, "CONOUT$", "w", stderr);
}

#if 0
static WCHAR *utf8_to_wchar(const char *string, int length, int *out_length) {
	WCHAR *buffer;
	int buf_length = MultiByteToWideChar(CP_UTF8, 0, string, length, NULL, 0);
	buffer = malloc(buf_length * sizeof(WCHAR)); /* TODO alloc function */
	MultiByteToWideChar(CP_UTF8, 0, string, length, buffer, buf_length);
	if (out_length)
		*out_length = buf_length;
	return buffer;
}
#endif

static char *wchar_to_utf8(const WCHAR *string, int length, int *out_length) {
	char *buffer;
	int buf_length = WideCharToMultiByte(CP_UTF8, 0, string, length, NULL, 0, 0, NULL);
	buffer = malloc(buf_length);
	WideCharToMultiByte(CP_UTF8, 0, string, length, buffer, buf_length, 0, NULL);
	if (out_length)
		*out_length = buf_length;
	return buffer;
}

/*
#ifdef ATTO_DEBUG
static void a__message_and_exit(const char *message, const char *file, int line) {
	int err = GetLastError();
	MessageBox(0, utf8_to_wchar(message, -1, NULL), utf8_to_wchar(file, -1, NULL), MB_ICONSTOP | MB_OK);
	ExitProcess(-1);
}
#define ATTO_ASSERT(cond, msg) if (!(cond)) a__message_and_exit(msg, __FILE__, __LINE__)
#else
#define ATTO_ASSERT(cond, msg) {}
#endif
*/

static AKey a__AppMapKey(WPARAM key) {
	if (key >= 'A' && key <= 'Z')
		return AK_A + key - 'A';
	if (key >= '0' && key <= '9')
		return AK_A + key - '0';

	switch (key) {
	case VK_BACK: return AK_Backspace;
	case VK_TAB: return AK_Tab;
	case VK_RETURN: return AK_Enter;
	case VK_LCONTROL: return AK_LeftCtrl;
	case VK_RCONTROL: return AK_RightCtrl;
	case VK_LMENU: return AK_LeftAlt;
	case VK_RMENU: return AK_RightAlt;
	case VK_CAPITAL: return AK_Capslock;
	case VK_ESCAPE: return AK_Esc;
	case VK_SPACE: return AK_Space;
	case VK_HOME: return AK_Home;
	case VK_LEFT: return AK_Left;
	case VK_UP: return AK_Up;
	case VK_RIGHT: return AK_Right;
	case VK_DOWN: return AK_Down;
	/* case VK_SNAPSHOT: return AK_Printscreen; */
	case VK_INSERT: return AK_Ins;
	case VK_DELETE: return AK_Del;
	case VK_SHIFT: return AK_LeftShift;
	case VK_LSHIFT: return AK_LeftShift;
	case VK_RSHIFT: return AK_RightShift;
	case VK_F1: return AK_F1;
	case VK_F2: return AK_F2;
	case VK_F3: return AK_F3;
	case VK_F4: return AK_F4;
	case VK_F5: return AK_F5;
	case VK_F6: return AK_F6;
	case VK_F7: return AK_F7;
	case VK_F8: return AK_F8;
	case VK_F9: return AK_F9;
	case VK_F10: return AK_F10;
	case VK_F11: return AK_F11;
	case VK_F12: return AK_F12;
	case VK_OEM_MINUS: return AK_Minus;
	case VK_OEM_PLUS: return AK_Plus;
	}
	return AK_Unknown;
}

static LRESULT CALLBACK a__AppWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	int down = 0;
	AKey key;

	enum {
		NoEvent,
		MouseClickEvent,
	} late_event = NoEvent;
	unsigned int new_button_state = a__app_state.pointer.buttons;

	switch (msg) {
	case WM_SIZE: {
		const unsigned int oldw = a__app_state.width, oldh = a__app_state.height;
		const int width = (int)(lparam & 0xffff), height = (int)(lparam >> 16);
		if (a__app_state.width == width && a__app_state.height == height)
			break;
		a__app_state.width = width;
		a__app_state.height = height;
		if (a__app_proctable.resize)
			a__app_proctable.resize(aAppTime(), oldw, oldh);
	} break;

	case WM_KEYDOWN: down = 1;
	case WM_KEYUP:
		key = a__AppMapKey(wparam);
		if (key == AK_Unknown)
			break;

		if (a__app_state.keys[key] == down)
			break;

		a__app_state.keys[key] = down;

		if (a__app_proctable.key)
			a__app_proctable.key(aAppTime(), key, down);
		break;

	case WM_CLOSE: ExitProcess(0); break;

	case WM_LBUTTONDOWN:
		late_event = MouseClickEvent;
		new_button_state |= AB_Left;
		break;
	case WM_LBUTTONUP:
		late_event = MouseClickEvent;
		new_button_state &= ~AB_Left;
		break;
	case WM_RBUTTONDOWN:
		late_event = MouseClickEvent;
		new_button_state |= AB_Right;
		break;
	case WM_RBUTTONUP:
		late_event = MouseClickEvent;
		new_button_state &= ~AB_Right;
		break;

	case WM_MOUSEMOVE:
		if (!a__app_state.grabbed) {
			const int x = GET_X_LPARAM(lparam), y = GET_Y_LPARAM(lparam);
			const int dx = x - a__app_state.pointer.x, dy = y - a__app_state.pointer.y;
			a__app_state.pointer.x = x;
			a__app_state.pointer.y = y;

			if (a__app_proctable.pointer)
				a__app_proctable.pointer(aAppTime(), dx, dy, 0);
		}
		break;

	case WM_INPUT: {
		RAWINPUT ri;
		UINT ri_size = sizeof(ri);
		const HRAWINPUT hrawinput = (HRAWINPUT)lparam;
		GetRawInputData(hrawinput, RID_INPUT, &ri, &ri_size, sizeof(RAWINPUTHEADER));

		switch (ri.header.dwType) {
		case RIM_TYPEMOUSE:
			if (!a__app_proctable.pointer)
				break;

			/*aAppDebugPrintf("%02x %u %04x %u %04x %d %d %u",
				ri.data.mouse.usFlags,
				ri.data.mouse.ulButtons,
				ri.data.mouse.usButtonFlags,
				ri.data.mouse.usButtonData,
				ri.data.mouse.ulRawButtons,
				ri.data.mouse.lLastX,
				ri.data.mouse.lLastY,
				ri.data.mouse.ulExtraInformation);*/

			if (ri.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
				if (g.rawMouse.resetAbsolute)
					g.rawMouse.resetAbsolute = 0;
				else {
					// TODO check on other absolute devices besides Synergy
					const int dx = (ri.data.mouse.lLastX - g.rawMouse.x) * GetSystemMetrics(SM_CXSCREEN) / 65536;
					const int dy = (ri.data.mouse.lLastY - g.rawMouse.y) * GetSystemMetrics(SM_CYSCREEN) / 65536;

					a__app_proctable.pointer(aAppTime(), dx, dy, 0);
				}

				g.rawMouse.x = ri.data.mouse.lLastX;
				g.rawMouse.y = ri.data.mouse.lLastY;
			} else if (ri.data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
				g.rawMouse.resetAbsolute = 1;
				a__app_proctable.pointer(aAppTime(), ri.data.mouse.lLastX, ri.data.mouse.lLastY, 0);
			}

			break;
		}
	} break;

	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	if (late_event == MouseClickEvent && new_button_state != a__app_state.pointer.buttons) {
		const unsigned int btn_diff = a__app_state.pointer.buttons ^ new_button_state;
		a__app_state.pointer.buttons = new_button_state;

		if (a__app_proctable.pointer)
			a__app_proctable.pointer(aAppTime(), 0, 0, btn_diff);
	}
	return 0;
}

void aAppGrabInput(int grab) {
	RAWINPUTDEVICE devices[] = {
		{1, 2, grab ? RIDEV_NOLEGACY /*| RIDEV_CAPTUREMOUSE*/ : RIDEV_REMOVE, grab ? g.hwnd : NULL}, /* mouse */
		/*{1, 6, RIDEV_NOLEGACY, NULL},*/ /* keyboard */
	};

	if (grab == a__app_state.grabbed)
		return;

	if (!RegisterRawInputDevices(devices, _countof(devices), sizeof(*devices))) {
		aAppDebugPrintf("Failed to register raw input devices");
		return;
	}

	g.rawMouse.resetAbsolute = 1;

	if (grab) {
		RECT r;
		GetClientRect(g.hwnd, &r);
		ClipCursor(&r);
		ShowCursor(FALSE);
	} else {
		ClipCursor(NULL);
		ShowCursor(TRUE);
	}

	a__app_state.grabbed = grab;
}
