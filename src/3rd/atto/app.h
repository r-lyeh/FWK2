#ifndef ATTO_APP_H__DECLARED
#define ATTO_APP_H__DECLARED

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int ATimeUs;

#ifndef _WIN32
	#define PRINTF_ARGS(a, b) __attribute__((format(printf, a, b)))
#else
	#define PRINTF_ARGS(a, b)
#endif

ATimeUs aAppTime(void);
void aAppDebugPrintf(const char *fmt, ...) PRINTF_ARGS(1, 2);
/* Immediately terminate current process */
void aAppTerminate(int code);

#define ATTO_ASSERT(cond) \
	if (!(cond)) { \
		aAppDebugPrintf("ERROR @ %s:%d: (%s) failed", __FILE__, __LINE__, #cond); \
		aAppTerminate(-1); \
	}

typedef enum {
	AK_Unknown = 0,
	AK_Backspace = 8,
	AK_Tab = 9,
	AK_Enter = 13,
	AK_Space = ' ',
	AK_Esc = 27,
	AK_PageUp = 33,
	AK_PageDown = 34,
	AK_Left = 37,
	AK_Up = 38,
	AK_Right = 39,
	AK_Down = 40,
	AK_Asterisk = '*',
	AK_Plus = '+',
	AK_Comma = ',',
	AK_Minus = '-',
	AK_Dot = '.',
	AK_Slash = '/',
	AK_0 = '0',
	AK_1 = '1',
	AK_2 = '2',
	AK_3 = '3',
	AK_4 = '4',
	AK_5 = '5',
	AK_6 = '6',
	AK_7 = '7',
	AK_8 = '8',
	AK_9 = '9',
	AK_Equal = '=',
	AK_A = 'a',
	AK_B = 'b',
	AK_C = 'c',
	AK_D = 'd',
	AK_E = 'e',
	AK_F = 'f',
	AK_G = 'g',
	AK_H = 'h',
	AK_I = 'i',
	AK_J = 'j',
	AK_K = 'k',
	AK_L = 'l',
	AK_M = 'm',
	AK_N = 'n',
	AK_O = 'o',
	AK_P = 'p',
	AK_Q = 'q',
	AK_R = 'r',
	AK_S = 's',
	AK_T = 't',
	AK_U = 'u',
	AK_V = 'v',
	AK_W = 'w',
	AK_X = 'x',
	AK_Y = 'y',
	AK_Z = 'z',
	AK_Tilda = '~',
	AK_Del = 127,
	AK_Ins,
	AK_LeftAlt,
	AK_LeftCtrl,
	AK_LeftMeta,
	AK_LeftSuper,
	AK_LeftShift,
	AK_RightAlt,
	AK_RightCtrl,
	AK_RightMeta,
	AK_RightSuper,
	AK_RightShift,
	AK_F1,
	AK_F2,
	AK_F3,
	AK_F4,
	AK_F5,
	AK_F6,
	AK_F7,
	AK_F8,
	AK_F9,
	AK_F10,
	AK_F11,
	AK_F12,
	AK_KeypadAsterisk,
	AK_KeypadPlus,
	AK_KeypadMinus,
	AK_Home,
	AK_End,
	AK_Capslock,
	AK_Max = 256
} AKey;

typedef enum {
	AB_Left = 1 << 0,
	AB_Right = 1 << 1,
	AB_Middle = 1 << 2,
	AB_WheelUp = 1 << 3,
	AB_WheelDown = 1 << 4
} AButton;

typedef enum { AOGLV_21, AOGLV_ES_20 } AOpenGLVersion;

struct AAppState {
	int argc;
	const char *const *argv;
	AOpenGLVersion gl_version;
	unsigned int width, height;
	int keys[AK_Max];
	struct {
		int x, y;
		unsigned int buttons;
	} pointer;
	int grabbed;
};

/* hide cursor, pin mouse to window center and report only delta */
void aAppGrabInput(int grab);

extern const struct AAppState *a_app_state;

struct AAppProctable {
	void (*resize)(ATimeUs ts, unsigned int old_width, unsigned int old_height);
	void (*paint)(ATimeUs ts, float dt);
	void (*key)(ATimeUs ts, AKey key, int down);
	void (*pointer)(ATimeUs ts, int dx, int dy, unsigned int buttons_changed_bits);
	void (*close)();
};

#ifndef ATTO_APP_INIT_FUNC
	#define ATTO_APP_INIT_FUNC attoAppInit
#endif /* ATTO_APP_INIT */

/* An application using atto app must implement this function and set the
 * relevant function pointers in proctable
 * atto/app guarantees that this and all other functions:
 *  - are be called from one thread only
 *  - always have a valid OpenGL context set for the thread
 *  - the first resize() will always precede the first paint()
 */
void ATTO_APP_INIT_FUNC(struct AAppProctable *proctable);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef ATTO_APP_H__DECLARED */
