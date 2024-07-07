#include "atto/app.h"
#define ATTO_GL_H_IMPLEMENT
#include "atto/gl.h"
#include "atto/math.h"

#include <math.h>

static void keyPress(ATimeUs timestamp, AKey key, int pressed) {
	(void)(timestamp);
	(void)(pressed);
	if (key == AK_Esc)
		aAppTerminate(0);
}

static const char shader_vertex[] =
	"uniform mat4 um4_vp, um4_model;\n"
	"attribute vec3 av3_pos;\n"
	"varying vec3 vv3_color;\n"
	"void main() {\n"
	"  vec4 pos = vec4(av3_pos, 1.);\n"
	"  vv3_color = av3_pos * .5 + .5;\n"
	"  gl_Position = um4_vp * um4_model * pos;\n"
	"}";

static const char shader_fragment[] =
	"varying vec3 vv3_color;\n"
	"void main() {\n"
	"  gl_FragColor = vec4(vv3_color, 1.);\n"
	"}";

static const struct AVec3f vertexes[8] = {
	{1.f, -1.f, 1.f},
	{1.f, 1.f, 1.f},
	{1.f, -1.f, -1.f},
	{1.f, 1.f, -1.f},
	{-1.f, -1.f, -1.f},
	{-1.f, 1.f, -1.f},
	{-1.f, -1.f, 1.f},
	{-1.f, 1.f, 1.f},
};

// clang-format off
static const unsigned short indices[36] = {
	0, 3, 1, 0, 2, 3,
	2, 5, 3, 2, 4, 5,
	4, 7, 5, 4, 6, 7,
	6, 1, 7, 6, 0, 1,
	6, 2, 0, 6, 4, 2,
	1, 5, 7, 1, 3, 5,
};
// clang-format on

static struct {
	AGLAttribute attr[1];
	AGLProgramUniform pun[2];
	AGLDrawSource draw;
	AGLDrawMerge merge;
	AGLDrawTarget target;
	struct AMat4f projection;
} g;

static void init(void) {
	g.draw.program = aGLProgramCreateSimple(shader_vertex, shader_fragment);
	if (g.draw.program <= 0) {
		aAppDebugPrintf("shader error: %s", a_gl_error);
		/* \fixme add fatal */
	}

	g.attr[0].name = "av3_pos";
	g.attr[0].buffer = 0;
	g.attr[0].size = 3;
	g.attr[0].type = GL_FLOAT;
	g.attr[0].normalized = GL_FALSE;
	g.attr[0].stride = 0;
	g.attr[0].ptr = vertexes;

	g.pun[0].name = "um4_vp";
	g.pun[0].type = AGLAT_Mat4;
	g.pun[0].count = 1;

	g.pun[1].name = "um4_model";
	g.pun[1].type = AGLAT_Mat4;
	g.pun[1].count = 1;

	g.draw.primitive.mode = GL_TRIANGLES;
	g.draw.primitive.count = 36;
	g.draw.primitive.first = -1;
	g.draw.primitive.index.buffer = 0;
	g.draw.primitive.index.data.ptr = indices;
	g.draw.primitive.index.type = GL_UNSIGNED_SHORT;

	g.draw.attribs.p = g.attr;
	g.draw.attribs.n = sizeof g.attr / sizeof *g.attr;

	aGLAttributeLocate(g.draw.program, g.attr, g.draw.attribs.n);

	g.draw.uniforms.p = g.pun;
	g.draw.uniforms.n = sizeof g.pun / sizeof *g.pun;

	aGLUniformLocate(g.draw.program, g.pun, g.draw.uniforms.n);

	g.merge.blend.enable = 0;
	g.merge.depth.mode = AGLDM_TestAndWrite;
	g.merge.depth.func = AGLDF_Less;
}

static void resize(ATimeUs timestamp, unsigned int old_w, unsigned int old_h) {
	(void)(timestamp);
	(void)(old_w);
	(void)(old_h);
	g.target.viewport.x = g.target.viewport.y = 0;
	g.target.viewport.w = a_app_state->width;
	g.target.viewport.h = a_app_state->height;

	g.target.framebuffer = 0;

	const float dpm = 38e3f; /* ~96 dpi */
	g.projection = aMat4fPerspective(.1f, 100.f, a_app_state->width / dpm, a_app_state->height / dpm);
}

static void paint(ATimeUs timestamp, float dt) {
	float t = timestamp * 1e-6f;
	AGLClearParams clear;
	(void)(dt);

	clear.a = 1;
	clear.r = .3f * sinf(t * .1f);
	clear.g = .3f * sinf(t * .2f);
	clear.b = .3f * sinf(t * .3f);
	clear.depth = 1;
	clear.bits = AGLCB_Everything;

	aGLClear(&clear, &g.target);

	struct AReFrame frame;
	frame.orient = aQuatRotation(aVec3fNormalize(aVec3f(.7f * sinf(t * .37f), .2f, .7f)), -t * .4f);
	frame.transl = aVec3f(0, 0, 0);

	struct AMat4f model4 = aMat4fReFrame(frame), vp4 = aMat4fMul(g.projection, aMat4fTranslation(aVec3f(0, 0, -10)));
	g.pun[1].value.pf = &model4.X.x;
	g.pun[0].value.pf = &vp4.X.x;

	aGLDraw(&g.draw, &g.merge, &g.target);
}

void attoAppInit(struct AAppProctable *proctable) {
	aGLInit();
	init();

	proctable->resize = resize;
	proctable->paint = paint;
	proctable->key = keyPress;
}
