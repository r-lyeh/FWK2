#include "atto/app.h"
#define ATTO_GL_H_IMPLEMENT
#include "atto/gl.h"
#include "atto/math.h"

#include <math.h>
#include <stdlib.h> /* malloc */

static void keyPress(ATimeUs timestamp, AKey key, int pressed) {
	(void)(timestamp);
	(void)(pressed);
	if (key == AK_Esc)
		aAppTerminate(0);
}

static const char shader_vertex[] =
	"uniform mat4 um4_vp, um4_model;\n"
	"uniform vec3 uv3_lightpos;\n"
	"attribute vec3 av3_pos, av3_normal, av3_color, av3_tricenter;\n"
	"varying vec3 vv3_color;\n"
	"void main() {\n"
	"  vec4 pos = um4_model * vec4(av3_pos, 1.);\n"
	"  vec3 normal = (um4_model * vec4(av3_normal, 0.)).xyz;\n"
	"  vec3 ldir = uv3_lightpos - pos.xyz;"
	"  vv3_color = av3_color * max(0., dot(normalize(ldir), normal)) / dot(ldir,ldir);\n"
	"  gl_Position = um4_vp * pos;\n"
	"}";

static const char shader_fragment[] =
	"varying vec3 vv3_color;\n"
	"void main() {\n"
	"  gl_FragColor = vec4(vv3_color, 1.);\n"
	"}";

const unsigned int triangles = 8192 * 2;

struct TriVertex {
	struct AVec3f pos, tricenter, normal, color;
};

typedef enum { VAttrPos, VAttrTriCenter, VAttrNormal, VAttrColor, VAttr_COUNT } VAttr;
typedef enum { VUniVP, VUniModel, VUniLightDir, VUni_COUNT } VUni;
static struct {
	AGLAttribute attr[VAttr_COUNT];
	AGLProgramUniform pun[VUni_COUNT];
	AGLDrawSource draw;
	AGLDrawMerge merge;
	AGLDrawTarget target;
	struct AMat4f projection;

	struct TriVertex *vertices;
	unsigned int vertices_count;
} g;

static struct AVec3f randomVector(struct ALCGRand *rng, float scale) {
	return aVec3fMulf(aVec3fSubf(aVec3f(aLcgRandf(rng), aLcgRandf(rng), aLcgRandf(rng)), .5f), 2.f * scale);
}

static void generateTriangles(unsigned int triangles) {
	struct ALCGRand rng = {triangles};

	g.vertices_count = triangles * 6;
	g.vertices = malloc(sizeof(*g.vertices) * g.vertices_count);
	for (unsigned int i = 0; i < triangles; ++i) {
		struct TriVertex *v = g.vertices + i * 6;
		const struct AVec3f center = randomVector(&rng, 1.f);
		v[0].tricenter = v[1].tricenter = v[2].tricenter = center;
		v[0].pos = aVec3fAdd(center, randomVector(&rng, .1f));
		v[1].pos = aVec3fAdd(center, randomVector(&rng, .1f));
		v[2].pos = aVec3fAdd(center, randomVector(&rng, .1f));
		v[0].normal = v[1].normal = v[2].normal =
			aVec3fNormalize(aVec3fCross(aVec3fSub(v[0].pos, v[1].pos), aVec3fSub(v[2].pos, v[1].pos)));
		v[0].color = v[1].color = v[2].color = aVec3f(aLcgRandf(&rng), aLcgRandf(&rng), aLcgRandf(&rng));

		v[3] = v[0];
		v[4] = v[2];
		v[5] = v[1];
		v[3].normal = v[4].normal = v[5].normal = aVec3fMulf(v[0].normal, -1.f);
	}
}

static void init(void) {
	generateTriangles(triangles);

	g.draw.program = aGLProgramCreateSimple(shader_vertex, shader_fragment);
	if (g.draw.program <= 0) {
		aAppDebugPrintf("shader error: %s", a_gl_error);
		/* \fixme add fatal */
	}

	g.attr[VAttrPos].name = "av3_pos";
	g.attr[VAttrPos].buffer = NULL;
	g.attr[VAttrPos].size = 3;
	g.attr[VAttrPos].type = GL_FLOAT;
	g.attr[VAttrPos].normalized = GL_FALSE;
	g.attr[VAttrPos].stride = sizeof(*g.vertices);
	g.attr[VAttrPos].ptr = &g.vertices[0].pos;

	g.attr[VAttrNormal].name = "av3_normal";
	g.attr[VAttrNormal].buffer = NULL;
	g.attr[VAttrNormal].size = 3;
	g.attr[VAttrNormal].type = GL_FLOAT;
	g.attr[VAttrNormal].normalized = GL_FALSE;
	g.attr[VAttrNormal].stride = sizeof(*g.vertices);
	g.attr[VAttrNormal].ptr = &g.vertices[0].normal;

	g.attr[VAttrTriCenter].name = "av3_tricenter";
	g.attr[VAttrTriCenter].buffer = NULL;
	g.attr[VAttrTriCenter].size = 3;
	g.attr[VAttrTriCenter].type = GL_FLOAT;
	g.attr[VAttrTriCenter].normalized = GL_FALSE;
	g.attr[VAttrTriCenter].stride = sizeof(*g.vertices);
	g.attr[VAttrTriCenter].ptr = &g.vertices[0].tricenter;

	g.attr[VAttrColor].name = "av3_color";
	g.attr[VAttrColor].buffer = NULL;
	g.attr[VAttrColor].size = 3;
	g.attr[VAttrColor].type = GL_FLOAT;
	g.attr[VAttrColor].normalized = GL_FALSE;
	g.attr[VAttrColor].stride = sizeof(*g.vertices);
	g.attr[VAttrColor].ptr = &g.vertices[0].color;

	g.pun[VUniVP].name = "um4_vp";
	g.pun[VUniVP].type = AGLAT_Mat4;
	g.pun[VUniVP].count = 1;

	g.pun[VUniModel].name = "um4_model";
	g.pun[VUniModel].type = AGLAT_Mat4;
	g.pun[VUniModel].count = 1;

	g.pun[VUniLightDir].name = "uv3_lightpos";
	g.pun[VUniLightDir].type = AGLAT_Vec3;
	g.pun[VUniLightDir].count = 1;

	g.draw.primitive.mode = GL_TRIANGLES;
	g.draw.primitive.count = g.vertices_count;
	g.draw.primitive.first = 0;
	g.draw.primitive.index.buffer = NULL;
	g.draw.primitive.index.data.ptr = NULL;
	g.draw.primitive.cull_mode = AGLCM_Front;
	g.draw.primitive.front_face = AGLFF_CounterClockwise;

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

	g.target.framebuffer = NULL;

	const float dpm = 38e3f; /* ~96 dpi */
	g.projection = aMat4fPerspective(.1f, 100.f, a_app_state->width / dpm, a_app_state->height / dpm);
}

static void paint(ATimeUs timestamp, float dt) {
	float t = timestamp * 1e-6f;
	AGLClearParams clear;
	(void)(dt);

	clear.a = 1;
	clear.r = .0f * sinf(t * .1f);
	clear.g = .0f * sinf(t * .2f);
	clear.b = .0f * sinf(t * .3f);
	clear.depth = 1;
	clear.bits = AGLCB_Everything;

	aGLClear(&clear, &g.target);

	struct AVec3f lpos = aVec3f(1, 0, 0);
	g.pun[VUniLightDir].value.pf = &lpos.x;

	struct AReFrame frame;
	frame.orient = aQuatRotation(aVec3fNormalize(aVec3f(.7f * sinf(t * .37f), .2f, .7f)), -t * .4f);
	// frame.orient = aQuatRotation(aVec3fNormalize(aVec3f(1, 1, .6)), t*.1f);
	frame.transl = aVec3f(0, 0, 0);

	struct AMat4f model4 = aMat4fReFrame(frame), vp4 = aMat4fMul(g.projection, aMat4fTranslation(aVec3f(0, 0, -10)));
	g.pun[VUniModel].value.pf = &model4.X.x;
	g.pun[VUniVP].value.pf = &vp4.X.x;

	const unsigned int split = 8192 * 3;
	for (unsigned int i = 0; i < g.vertices_count / split; ++i) {
		g.draw.primitive.first = split * i;
		g.draw.primitive.count = split;
		aGLDraw(&g.draw, &g.merge, &g.target);
	}
}

void attoAppInit(struct AAppProctable *proctable) {
	aGLInit();
	init();

	proctable->resize = resize;
	proctable->paint = paint;
	proctable->key = keyPress;
}
