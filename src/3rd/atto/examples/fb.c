#include <math.h>
#include <atto/app.h>

#define ATTO_GL_H_IMPLEMENT
#include <atto/gl.h>

static const char shader_vertex[] =
	"attribute vec2 av2_pos;"
	"varying vec2 vv2_pos;"
	"void main() { vv2_pos = av2_pos; gl_Position = vec4(av2_pos, 0., 1.); }";

static const char shader_fragment[] =
	"uniform float uf_time;"
	"varying vec2 vv2_pos;"
	"void main() {"
	"  float a = 5. * atan(vv2_pos.x, vv2_pos.y);"
	"  float r = 10. * sin(uf_time + length(vv2_pos)*6.);"
	"  gl_FragColor = abs(vec4("
	"    sin(a+uf_time*4.+r),"
	"    sin(2.+a+uf_time*3.+r),"
	"    sin(4.+a+uf_time*5.+r),"
	"    1.));"
	"}";

static const char shader_fragment_show[] =
	"uniform sampler2D us2_texture;"
	"uniform float uf_time;"
	"varying vec2 vv2_pos;"
	"void main() {"
	"  float a = 5. * atan(vv2_pos.x, vv2_pos.y);"
	"  float r = .1 * sin(.4 * uf_time + length(vv2_pos)*3.);"
	"  gl_FragColor = "
	"    texture2D(us2_texture, vv2_pos*.5 - vec2(.5) + r*vec2(sin(a),cos(a)));"
	"}";

// clang-format off
static const float vertexes[] = {
	1.f, -1.f,
	0.f, 1.f,
	-1.f, -1.f
};
static const float screenquad[] = {
	1.f, -1.f,
	1.f, 1.f,
	-1.f, -1.f,
	-1.f, 1.f,
};
// clang-format on

static struct {
	AGLAttribute attr[1];
	AGLProgramUniform uni[2];
	AGLAttribute shattr[1];

	AGLDrawSource draw, show;
	AGLDrawMerge merge;
	AGLDrawTarget screen, fb;

	AGLTexture fbtex;
	AGLFramebufferParams fbp;
	AGLClearParams clear;
} g;

static void init(void) {
	g.draw.program = aGLProgramCreateSimple(shader_vertex, shader_fragment);
	if (g.draw.program <= 0) {
		aAppDebugPrintf("shader error: %s", a_gl_error);
		/* \fixme add fatal */
	}

	g.show.program = aGLProgramCreateSimple(shader_vertex, shader_fragment_show);
	if (g.show.program <= 0) {
		aAppDebugPrintf("shader error: %s", a_gl_error);
		/* \fixme add fatal */
	}

	g.fbtex = aGLTextureCreate();

	g.clear.r = g.clear.g = g.clear.b = g.clear.a = 0;
	g.clear.depth = 1;
	g.clear.bits = AGLCB_Everything;

	g.attr[0].name = "av2_pos";
	g.attr[0].buffer = 0;
	g.attr[0].size = 2;
	g.attr[0].type = GL_FLOAT;
	g.attr[0].normalized = GL_FALSE;
	g.attr[0].stride = 0;
	g.attr[0].ptr = vertexes;

	g.uni[0].name = "uf_time";
	g.uni[0].type = AGLAT_Float;
	g.uni[0].count = 1;

	g.draw.primitive.mode = GL_TRIANGLES;
	g.draw.primitive.count = 3;
	g.draw.primitive.first = 0;
	g.draw.primitive.index.buffer = 0;
	g.draw.primitive.index.data.ptr = 0;
	g.draw.primitive.index.type = 0;

	g.draw.attribs.p = g.attr;
	g.draw.attribs.n = sizeof g.attr / sizeof *g.attr;

	g.draw.uniforms.p = g.uni;
	g.draw.uniforms.n = 1;

	g.fbp.depth.texture = 0;
	g.fbp.depth.mode = AGLDBM_Texture;
	g.fbp.color = &g.fbtex;

	g.shattr[0].name = "av2_pos";
	g.shattr[0].buffer = 0;
	g.shattr[0].size = 2;
	g.shattr[0].type = GL_FLOAT;
	g.shattr[0].normalized = GL_FALSE;
	g.shattr[0].stride = 0;
	g.shattr[0].ptr = screenquad;

	g.uni[1].name = "us2_texture";
	g.uni[1].type = AGLAT_Texture;
	g.uni[1].value.texture = &g.fbtex;
	g.uni[1].count = 1;

	g.show.primitive.mode = GL_TRIANGLE_STRIP;
	g.show.primitive.count = 4;
	g.show.primitive.first = 0;
	g.show.primitive.index.buffer = 0;
	g.show.primitive.index.data.ptr = 0;
	g.show.primitive.index.type = 0;

	g.show.attribs.p = g.shattr;
	g.show.attribs.n = sizeof g.shattr / sizeof *g.shattr;

	g.show.uniforms.p = g.uni;
	g.show.uniforms.n = sizeof g.uni / sizeof *g.uni;

	g.screen.framebuffer = 0;
	g.fb.framebuffer = &g.fbp;

	g.merge.blend.enable = 0;
	g.merge.depth.mode = AGLDM_Disabled;
}

static void resize(ATimeUs timestamp, unsigned int old_w, unsigned int old_h) {
	AGLTextureUploadData data;
	(void)(timestamp);
	(void)(old_w);
	(void)(old_h);

	data.format = AGLTF_U8_RGBA;
	data.x = data.y = 0;
	data.width = a_app_state->width;
	data.height = a_app_state->height;
	data.pixels = 0;
	aGLTextureUpload(&g.fbtex, &data);

	g.screen.viewport.x = 0;
	g.screen.viewport.y = 0;
	g.screen.viewport.w = a_app_state->width;
	g.screen.viewport.h = a_app_state->height;

	g.fb.viewport.x = 0;
	g.fb.viewport.y = 0;
	g.fb.viewport.w = a_app_state->width;
	g.fb.viewport.h = a_app_state->height;
}

static void paint(ATimeUs timestamp, float dt) {
	float t = timestamp * 1e-6f;
	(void)(dt);

	g.clear.r = sinf(t * .1f);
	g.clear.g = sinf(t * .2f);
	g.clear.b = sinf(t * .3f);

	aGLClear(&g.clear, &g.fb);

	g.uni[0].value.pf = &t;
	aGLDraw(&g.draw, &g.merge, &g.fb);
	aGLDraw(&g.show, &g.merge, &g.screen);
}

static void keyPress(ATimeUs timestamp, AKey key, int pressed) {
	(void)(timestamp);
	(void)(pressed);
	if (key == AK_Esc)
		aAppTerminate(0);
}

void attoAppInit(struct AAppProctable *proctable) {
	aGLInit();
	init();

	proctable->resize = resize;
	proctable->paint = paint;
	proctable->key = keyPress;
}
