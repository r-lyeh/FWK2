#include "atto/app.h"

#define ATTO_GL_H_IMPLEMENT
#include "atto/gl.h"

#include <math.h>

static void keyPress(ATimeUs timestamp, AKey key, int pressed) {
	(void)(timestamp);
	(void)(pressed);
	if (key == AK_Esc)
		aAppTerminate(0);
}

static const char shader_vertex[] =
	"uniform float uf_time;"
	"attribute vec2 av2_pos;\n"
	"varying vec3 vv3_color;\n"
	"void main() {\n"
	"  float a = 5. * atan(av2_pos.x, av2_pos.y);\n"
	"  vv3_color = abs(vec3(\n"
	"    sin(a+uf_time*4.),\n"
	"    sin(2.+a+uf_time*3.),\n"
	"    sin(4.+a+uf_time*5.)));\n"
	"  gl_Position = vec4(av2_pos, 0., 1.);\n"
	"}";

static const char shader_fragment[] =
	"varying vec3 vv3_color;\n"
	"void main() {\n"
	"  gl_FragColor = vec4(vv3_color, 1.);\n"
	"}";

// clang-format off
static const float vertexes[] = {
	1.f, -1.f,
	0.f, 1.f,
	-1.f, -1.f
};
// clang-format on

static struct {
	AGLAttribute attr[1];
	AGLProgramUniform pun[1];
	AGLDrawSource draw;
	AGLDrawMerge merge;
	AGLDrawTarget target;
} g;

static void init(void) {
	g.draw.program = aGLProgramCreateSimple(shader_vertex, shader_fragment);
	if (g.draw.program <= 0) {
		aAppDebugPrintf("shader error: %s", a_gl_error);
		/* \fixme add fatal */
	}

	g.attr[0].name = "av2_pos";
	g.attr[0].buffer = 0;
	g.attr[0].size = 2;
	g.attr[0].type = GL_FLOAT;
	g.attr[0].normalized = GL_FALSE;
	g.attr[0].stride = 0;
	g.attr[0].ptr = vertexes;

	g.pun[0].name = "uf_time";
	g.pun[0].type = AGLAT_Float;
	g.pun[0].count = 1;

	g.draw.primitive.mode = GL_TRIANGLES;
	g.draw.primitive.count = 3;
	g.draw.primitive.first = 0;
	g.draw.primitive.index.buffer = 0;
	g.draw.primitive.index.data.ptr = 0;
	g.draw.primitive.index.type = 0;

	g.draw.attribs.p = g.attr;
	g.draw.attribs.n = sizeof g.attr / sizeof *g.attr;

	aGLAttributeLocate(g.draw.program, g.attr, g.draw.attribs.n);

	g.draw.uniforms.p = g.pun;
	g.draw.uniforms.n = sizeof g.pun / sizeof *g.pun;

	aGLUniformLocate(g.draw.program, g.pun, g.draw.uniforms.n);

	g.merge.blend.enable = 0;
	g.merge.depth.mode = AGLDM_Disabled;
}

static void resize(ATimeUs timestamp, unsigned int old_w, unsigned int old_h) {
	(void)(timestamp);
	(void)(old_w);
	(void)(old_h);
	g.target.viewport.x = g.target.viewport.y = 0;
	g.target.viewport.w = a_app_state->width;
	g.target.viewport.h = a_app_state->height;

	g.target.framebuffer = 0;
}

static void paint(ATimeUs timestamp, float dt) {
	float t = timestamp * 1e-6f;
	AGLClearParams clear;
	(void)(dt);

	clear.a = 1;
	clear.r = sinf(t * .1f);
	clear.g = sinf(t * .2f);
	clear.b = sinf(t * .3f);
	clear.depth = 0;
	clear.bits = AGLCB_Everything;

	aGLClear(&clear, &g.target);

	g.pun[0].value.pf = &t;
	aGLDraw(&g.draw, &g.merge, &g.target);
}

void attoAppInit(struct AAppProctable *proctable) {
	aGLInit();
	init();

	proctable->resize = resize;
	proctable->paint = paint;
	proctable->key = keyPress;
}
