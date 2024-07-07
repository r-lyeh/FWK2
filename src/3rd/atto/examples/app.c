#include "atto/app.h"

static const char *opengl_version(AOpenGLVersion ver) {
	switch (ver) {
	case AOGLV_21: return "OpenGL 2.1 or later"; break;
	case AOGLV_ES_20: return "ES 2.0"; break;
	}
	return "Unknown";
}

static struct {
	ATimeUs time;
	float dt, min_dt, max_dt;
	int frames;
} fps = {0, 0, 0, 0, 0};

static void appResize(ATimeUs timestamp, unsigned int old_width, unsigned int old_height) {
	(void)(old_width);
	(void)(old_height);
	aAppDebugPrintf("%s[%u]: width = %d, height = %d", __func__, timestamp, a_app_state->width, a_app_state->height);
}

static void appPaint(ATimeUs timestamp, float dt) {
	fps.dt += dt;
	fps.min_dt = (fps.min_dt > dt || !fps.min_dt) ? dt : fps.min_dt;
	fps.max_dt = (fps.max_dt < dt) ? dt : fps.max_dt;
	++fps.frames;
	if (timestamp - fps.time > 1000000) {
		float sec = (timestamp - fps.time) * 1e-6f;
		aAppDebugPrintf(
			"%s[%u]: elapsed: %.2fs, frames = %d, "
			"min_dt = %.2fms, avg_dt = %.2fms, max_dt = %.2fms, avg_fps = %.2f",
			__func__, timestamp, sec, fps.frames, fps.min_dt * 1e3, fps.dt * 1e3 / fps.frames, fps.max_dt * 1e3,
			fps.frames / sec);
		fps.frames = 0;
		fps.min_dt = fps.dt = fps.max_dt = 0;
		fps.time = timestamp;
	}
}

static void appKeyPress(ATimeUs timestamp, AKey key, int down) {
	aAppDebugPrintf("%s[%u]: key = %d, down = %d", __func__, timestamp, key, down);

	if (key == AK_Esc)
		aAppTerminate(0);
}

static void appPointer(ATimeUs timestamp, int dx, int dy, unsigned int buttons_changed_bits) {
	aAppDebugPrintf("%s[%u]: x: %d, y: %d, btn: 0x%x, dx: %d, dy: %d, dbtn: 0x%x", __func__, timestamp,
		a_app_state->pointer.x, a_app_state->pointer.y, a_app_state->pointer.buttons, dx, dy, buttons_changed_bits);
}

static void appClose() {
	aAppDebugPrintf("%s", __func__);
}

void attoAppInit(struct AAppProctable *proctable) {
	int i;
	aAppDebugPrintf("%s[%u]: argc = %d, argv = %p", __func__, 0, a_app_state->argc, (void *)a_app_state->argv);
	for (i = 0; i < a_app_state->argc; ++i) aAppDebugPrintf("\targv[%d] = '%s'", i, a_app_state->argv[i]);
	aAppDebugPrintf("\tOpenGL version %s", opengl_version(a_app_state->gl_version));

	proctable->resize = appResize;
	proctable->paint = appPaint;
	proctable->key = appKeyPress;
	proctable->pointer = appPointer;
	proctable->close = appClose;
}
