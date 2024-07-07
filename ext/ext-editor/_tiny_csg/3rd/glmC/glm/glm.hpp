#pragma once
#include <string>

namespace glm {

	#ifndef __cplusplus
	#define vec3(...) ((vec3){__VA_ARGS__})
	#define vec4(...) ((vec4){__VA_ARGS__})
	#define mat3(...) ((mat3){__VA_ARGS__})
	#define mat4(...) ((mat4){__VA_ARGS__})
	#endif

	typedef struct vec4 { float x,y,z,w; } vec4;

	typedef struct vec3 { float x,y,z;
		vec3 operator-() { return vec3(-x,-y,-z); }
		vec3 operator-(const vec3 &a) { return vec3(x-a.x,y-a.y,z-a.z); }
		vec3 operator/(float a) { return vec3(x/a,y/a,z/a); }
		operator vec4() const { return vec4(x,y,z,0); }
	} vec3;
	typedef struct mat3 { vec3 a,b,c; 
		mat3 operator/(float n) { return mat3(a/n,b/n,c/n); }
	} mat3;
	typedef struct mat4 { vec4 a,b,c,d; } mat4;


	vec3 cross(vec3 a, vec3 b) { return vec3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y); }

	vec4 min(vec4 a, vec4 b) { return vec4((std::min)(a.x,b.x),(std::min)(a.y,b.y),(std::min)(a.z,b.z),(std::min)(a.w,b.w)); }
	vec4 max(vec4 a, vec4 b) { return vec4((std::max)(a.x,b.x),(std::max)(a.y,b.y),(std::max)(a.z,b.z),(std::max)(a.w,b.w)); }

	vec4 scale(vec4 a, float b) { return vec4(a.x * b, a.y * b, a.z * b, a.w * b); }
	float dot(vec4 a, vec4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
	float length2(vec4 a) { return dot(a,a); }
	float length(vec4 a) { return sqrtf(length2(a)); }
	vec4  normalize(vec4 a) { float len = length(a); return scale(a, 1 / (len+!len)); }

	bool all(vec4 a) { return !!a.x && !!a.y && !!a.z && !!a.w; }
	bool lessThanEqual(vec4 a, vec4 b) { return a.x <= b.x && a.y <= b.y && a.z <= b.z && a.w <= b.w; }
	bool greaterThanEqual(vec4 a, vec4 b) { return a.x >= b.x && a.y >= b.y && a.z >= b.z && a.w >= b.w; }

	std::string to_string(vec4 a) { char buf[128]; snprintf(buf,128,"%f %f %f %f", a.x,a.y,a.z,a.w); return buf; }

	const float *value_ptr(const vec3 &v) { return &v.x; }
	const float *value_ptr(const vec4 &v) { return &v.x; }
	const float *value_ptr(const mat3 &v) { return &v.a.x; }
	const float *value_ptr(const mat4 &v) { return &v.a.x; }

	// @fixme {
	void  row(mat3 m, int c, vec3 v) { (c==0 ? m.a : c==1 ? m.b : m.c) = v; }
	void  column(mat3 m, int c, vec3 v) { if(c==0) m.a.x=v.x,m.b.x=v.y,m.c.x=v.x; else if(c == 1) m.a.y=v.y,m.b.y=v.y,m.c.y=v.y; else m.a.z=v.z,m.b.z=v.z,m.c.z=v.z; }
	vec3  row(mat3 m, int c) { if(c==0) return m.a; return c == 1 ? m.b : m.c; }
	vec3  column(mat3 m, int c) { if(c==0) return vec3(m.a.x,m.b.x,m.c.x); return c == 1 ? vec3(m.a.y,m.b.y,m.c.y) : vec3(m.a.z,m.b.z,m.c.z); }
	vec4  rotate(vec4 v, float x) { return v; }
	mat3  determinant(mat3 m) { return m; }
	bool intersectRayPlane(vec3 orig, vec3 dir, vec3 planeOrig, vec3 planeNormal, float &intersectionDistance) {
		return 0;
	}
	// } @fixme

}
