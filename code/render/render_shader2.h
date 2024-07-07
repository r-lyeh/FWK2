// -----------------------------------------------------------------------------
// shaders v2

// @todo: permutations: all statics in a different worker thread
// @todo: permutations: integers 1,0, 16,8,4,2,1,0, 5,4,3,2,1,0
// @todo: permutations: floats values are not permuted: 0.001f ? maybe we could permute range extremes ie, [A,B] permute (A) and (B)
// @todo: save them to a binary file
// @todo: runtime, if a permutation is not ready, give that thread extra priority and wait in mainthread

// @todo: .ini containsall defines, attribs, outcolors, state of uniforms too
// @todo: .xxx is spirv code
// @spec: .shaders.zip/[hash_of_vs_fs_gs]/shader.ini
// @spec: .shaders.zip/[hash_of_vs_fs_gs]/shader.XXX

// @todo: shader2() would return a shader_t* pointer instead, then:
// static map(unsigned,shader_t) shader2_byid; // would search by program id, return instance
// static map(char*,unsigned) shader2_byname; // would search by name, return program id

// @todo: deprecate render_shader_bind.h file

// @todo: scan #define XXX, #ifdef XXX, #if XXX lines in shader source and append them to `defines` automtically for further permutations

/*
gs            \
vs             > shader id, also used to hash contents
fs            /

mutables     >>> static permutations. require shader reload

uniforms     >>> dynamic permutations
*/

#if !CODE

// uniforms -------------------------------------------------------------------

enum UNIFORM_KIND {
    UNIFORM_BOOL = 'b',
    UNIFORM_INT = 'i',
    UNIFORM_UINT = 'u',
    UNIFORM_FLOAT = 'x',
    UNIFORM_VEC2 = 'y',
    UNIFORM_VEC3 = 'z',
    UNIFORM_VEC4 = 'w',
    UNIFORM_MAT3 = 'm',
    UNIFORM_MAT4 = 'M',
    UNIFORM_MAT34 = 'n',
    UNIFORM_SAMPLER2D = 's',
    UNIFORM_SAMPLER3D = 'S',
    UNIFORM_SAMPLERCUBE = 'C',
};

typedef struct uniform_t {
    char type_name[64]; // uniform kind stored in [0], name in [1..63]
    int location, count, unit;
    void *data;
    union {
        float f;
        int i;
        bool b;
        unsigned u;
        vec2 v2;
        vec3 v3;
        vec4 v4;
        mat33 m33;
        mat34 m34;
        mat44 m44;
        char any[1];
    };
} uniform_t;

API uniform_t uniform(const char *type_name, uniform_t value);

#define uniform(name, ...) uniform( name, ((uniform_t){ {0},-1,1,0, NULL, { .f = 0, __VA_ARGS__ }}) )
#define uniform2 uniform

API void uniform_set2(uniform_t *, const void *data);
API void uniform_set4(uniform_t *, int index, int count, const void *data);

// shaders --------------------------------------------------------------------

typedef struct shader_t {

    unsigned program;
    uint64_t hash;

    // debugging
    char *gs, *vs, *fs;
    char *attribs, *fragcolors, *cdefines;

    // statics
    array(char*) constants; // immutable key=value pairs

    // hybrid
    array(char*) mutables; // mutable key=value pairs, that are permuted across different shader generations. keys must honor USE_XXX format
    bool reload;

    // dynamics
    array(uniform_t) uniforms;
    uint64_t dirty; // @todo: use dirty[2] or [3] for 128/192 uniforms instead

} shader_t;

shader_t shader2(const char *gs, const char *vs, const char *fs, const char *defines,
        const char *attribs, // to deprecate
        const char *fragcolors // to deprecate
        );
shader_t shader2_dup( const shader_t s );
void     shader2_setdefine(shader_t *s, const char *define, char digit);
void     shader2_adduniforms(shader_t* s, unsigned count, uniform_t* uniforms);
void     shader2_adduniform(shader_t* s, uniform_t uniform);
bool     shader2_apply(shader_t *s); // patch all uniforms that are dirty; will replace program if mutables are changed. returns true if program changed.
void     shader2_destroy(shader_t *s);

#else

// uniforms -------------------------------------------------------------------

uniform_t (uniform)(const char *name, uniform_t value) {
    int kind = 0;
    /**/ if( strbeg(name, "int "  ) ) kind = UNIFORM_INT,   name += 4;
    else if( strbeg(name, "bool " ) ) kind = UNIFORM_BOOL,  name += 5;
    else if( strbeg(name, "uint " ) ) kind = UNIFORM_INT,   name += 5; // @fixme?
    else if( strbeg(name, "vec2 " ) ) kind = UNIFORM_VEC2,  name += 5;
    else if( strbeg(name, "vec3 " ) ) kind = UNIFORM_VEC3,  name += 5;
    else if( strbeg(name, "vec4 " ) ) kind = UNIFORM_VEC4,  name += 5;
    else if( strbeg(name, "mat3 " ) ) kind = UNIFORM_MAT3,  name += 5;
    else if( strbeg(name, "mat4 " ) ) kind = UNIFORM_MAT4,  name += 5;
    else if( strbeg(name, "mat33 ") ) kind = UNIFORM_MAT3,  name += 6;
    else if( strbeg(name, "mat34 ") ) kind = UNIFORM_MAT34, name += 6;
    else if( strbeg(name, "mat44 ") ) kind = UNIFORM_MAT4,  name += 6;
    else if( strbeg(name, "float ") ) kind = UNIFORM_FLOAT, name += 6;
    else if( strbeg(name, "mat3x4 ") ) kind = UNIFORM_MAT34, name += 7;
    else if( strbeg(name, "sampler2D " ) ) kind = UNIFORM_SAMPLER2D, name += 10, value.unit = value.u;
    else if( strbeg(name, "sampler3D " ) ) kind = UNIFORM_SAMPLER3D, name += 10, value.unit = value.u;
    else if( strbeg(name, "samplerCube " ) ) kind = UNIFORM_SAMPLERCUBE, name += 12, value.unit = value.u;
    else alert(va("uniform not found %s", name));
    snprintf(value.type_name, sizeof(value.type_name), "%c%s", kind, name);
    char *at = strchr(value.type_name,'[');
    if(at) *at = '\0', value.count = atoi(at+1);
    return value;
}

void uniform_set4(uniform_t *u, int index, int count, const void *data) {
    if( u->location > -10 && u->location < 0 ) { // 10 retries max
        u->location--;

        GLint program = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
 
        char *s = u->type_name + 1;
        for( int i = 0; s && s[0]; ++i ) {
            char token[64] = {0};
            int rc = sscanf(s, "%63[^,]", token);
            while( s[0] && s[0] != ',' ) { s++; } // eat word
            while( s[0] && s[0] == ',' ) { s++; break; } // eat 1 comma
            int loc;
            if(token[0] && (loc = glGetUniformLocation(program, token)) >= 0) {
                u->location = loc;
                break;
            }
        }
    }
    if( u->location < 0 ) {
        igText("uniform [%c]%s not found\n", u->type_name[0], u->type_name + 1);
        return;
    }

    if( count == ~0u ) count = u->count;

    switch (u->type_name[0]) {
        default:
            die("unknown uniform type [%c]%s", u->type_name[0], u->type_name + 1);

        break; case UNIFORM_BOOL:           glUniform1i(u->location + index, *((const bool*)data)); // @fixme: handle `count`
        break; case UNIFORM_INT:            glUniform1iv(u->location + index, count, (const int*)data);
        break; case UNIFORM_UINT:           glUniform1uiv(u->location + index, count, (const unsigned*)data);
        break; case UNIFORM_FLOAT:          glUniform1fv(u->location + index, count, (const float*)data);
        break; case UNIFORM_VEC2:           glUniform2fv(u->location + index, count, (const float*)data);
        break; case UNIFORM_VEC3:           glUniform3fv(u->location + index, count, (const float*)data);
        break; case UNIFORM_VEC4:           glUniform4fv(u->location + index, count, (const float*)data);
        break; case UNIFORM_MAT3:           glUniformMatrix3fv(u->location + index, count, GL_FALSE, (const float*)data);
        break; case UNIFORM_MAT4:           glUniformMatrix4fv(u->location + index, count, GL_FALSE, (const float*)data);
        break; case UNIFORM_MAT34:          glUniformMatrix3x4fv(u->location + index, count, GL_FALSE, (const float*)data);
        break; case UNIFORM_SAMPLER2D: case UNIFORM_SAMPLER3D: case UNIFORM_SAMPLERCUBE: {
            const int kind[] = {
                [UNIFORM_SAMPLER2D] = GL_TEXTURE_2D,
                [UNIFORM_SAMPLER3D] = GL_TEXTURE_3D,
                [UNIFORM_SAMPLERCUBE] = GL_TEXTURE_CUBE_MAP,
            };
            if( data )
            for( int i = 0; i < count; ++i ) {
                glActiveTexture(GL_TEXTURE0 + u->unit + index + i);
                glBindTexture(kind[u->type_name[0]], ((const GLuint*)data + 0)[i]);
            }
            for( int i = 0; i < count; ++i ) {
                glUniform1i(u->location + index + i, u->unit + index + i);
            }
        }
    }
}

void uniform_set2(uniform_t *u, const void *data) {
    uniform_set4(u, 0, u->count, data);
}

// shaders --------------------------------------------------------------------

// @fixme: move to ds
#define array_dup(a, b) ( (b) ? memcpy( array_resize(a, array_count(b)), (b), array_count(b) * sizeof(0[b]) ) : (a) )

static
GLuint shader2_compile( GLenum type, array(char*) sources ) {
    if( (*array_back(sources))[0] == '\0' ) return 0; // do not compile empty shaders

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, array_count(sources), sources, NULL);
    glCompileShader(shader);

    GLint status = GL_FALSE, length;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if( status == GL_FALSE ) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        char *buf = stack(length+1);
        glGetShaderInfoLog(shader, length, NULL, buf);

        // dump log with line numbers
        for(int n = 0, e = array_count(sources); n < e; ++n) {
            const char *source = sources[n];
            for(int line = 0, i = 0; source[i] > 0; ) {
                printf("\t%d:%03d: ", n, line+1);
                while( source[i] >= 32 || source[i] == '\t' ) fputc(source[i++], stdout);
                while( source[i] > 0 && source[i] < 32 ) line += source[i++] == '\n';
                puts("");
            }
        }

        die("!ERROR: shader2_compile():\nDevice: %s\nDriver: %s\nShader/program link: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION), buf);
        return 0;
    }

    return shader;
}

static
unsigned shader2_compile_and_link(shader_t *z, int parent_reflection) {
    // assemble sections
    static array(char*) sections = 0; array_resize(sections, 0);

    const char *glsl_version = ifdef(ems, "#version 300 es\n", "#version 330\n");
    array_push(sections, (char*)glsl_version);

    // rebuild C defines section, which holds all our
    // defines in `#define XXX vvv\n" C fashion
    {
        FREE(z->cdefines); z->cdefines = NULL;

        for each_array(z->constants, char*, it) {
            char buffer[128]; snprintf(buffer, 128, "#define %s\n", it);
            char *at = strchr(buffer, '='); if(at) *at = ' ';
            strcatf(&z->cdefines, "%s", buffer);
        }

        for each_array(z->mutables, char*, it) {
            char buffer[128]; snprintf(buffer, 128, "#define %s\n", it);
            char *at = strchr(buffer, '='); if(at) *at = ' ';
            strcatf(&z->cdefines, "%s", buffer);
        }

        if(z->cdefines)
        array_push(sections, z->cdefines);
    }

    static char *preamble; do_once preamble = STRDUP( file_read("shaderlib/compat.glsl", 0) );
    array_push(sections, preamble);
    array_push(sections, "\n");

    array_push(sections, z->gs);
    GLuint geom = shader2_compile(GL_GEOMETRY_SHADER, sections);
    array_pop(sections);

    array_push(sections, z->vs);
    GLuint vert = shader2_compile(GL_VERTEX_SHADER, sections);
    array_pop(sections);

    array_push(sections, z->fs);
    GLuint frag = shader2_compile(GL_FRAGMENT_SHADER, sections);
    array_pop(sections);

    if(! ( vert && frag ) ) return -1;

    // compile & link
    unsigned program = glCreateProgram();

    if (geom)
    glAttachShader(program, geom);
    glAttachShader(program, vert);
    glAttachShader(program, frag);

    // iterate "/// attribs" tags
    {
        const char *line = z->attribs;
        //line = line ? line : strstr(z->gs,"/// attribs:") + 12;
          line = line ? line : strstr(z->vs,"/// attribs:") + 12;
        //line = line ? line : strstr(z->fs,"/// attribs:") + 12;

        char attribs[256] = {0};
        if( line && sscanf(line, "%255[^;\r\n]", attribs) ) {
            char *s = attribs;
            for( int i = 0; s && s[0]; ++i ) {
                char token[64] = {0};
                int rc = sscanf(s, "%63[^,]", token);
                while( s[0] && s[0] != ',' ) { s++; } // eat word
                while( s[0] && s[0] == ',' ) { s++; break; } // eat 1 comma
                if(token[0]) glBindAttribLocation(program, i, token);
            }
        }
    }

    // iterate "/// fragcolors" tags
    {
        const char *line = z->fragcolors;
        //line = line ? line : strstr(z->gs,"/// fragcolors:") + 15;
          line = line ? line : strstr(z->vs,"/// fragcolors:") + 15;
        //line = line ? line : strstr(z->fs,"/// fragcolors:") + 15;

        char fragcolors[256] = {0};
        if( line && sscanf(line, "%255[^;\r\n]", fragcolors) ) {
            char *s = fragcolors;
            for( int i = 0; s && s[0]; ++i ) {
                char token[64] = {0};
                int rc = sscanf(s, "%63[^,]", token);
                while( s[0] && s[0] != ',' ) { s++; } // eat word
                while( s[0] && s[0] == ',' ) { s++; break; } // eat 1 comma
                if(token[0]) glBindFragDataLocation(program, i, token);
            }
        }
    }

    glLinkProgram(program);

    GLint status = GL_FALSE, length;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        char *buf = stack(length+1);
        glGetProgramInfoLog(program, length, NULL, buf);

        if (geom)
        puts("--- gs:"), shader_print(z->gs);
        puts("--- vs:"), shader_print(z->vs);
        puts("--- fs:"), shader_print(z->fs);

        die("ERROR: shader():\nDevice: %s\nDriver: %s\nShader/program link: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION), buf);
    }

    if (geom)
    glDeleteShader(geom);
    glDeleteShader(vert);
    glDeleteShader(frag);

    glFinish(); // needed?

    z->program = program;

    // shader compiled fine, before returning, let's parse the source and reflect the uniforms
    do_once map_init_int( shader_reflect );
    array(char*) props = 0;
    if( parent_reflection < 0 ) {
        if(z->vs) for each_substring(z->vs, "\r\n", line) {
            const char *found = strstr(line, "/""//");
            if( found > line && line[0] == '/' && line[1] == '/' ) continue;
            if( found ) array_push(props, STRDUP(line));
        }
        if(z->fs) for each_substring(z->fs, "\r\n", line) {
            const char *found = strstr(line, "/""//");
            if( found > line && line[0] == '/' && line[1] == '/' ) continue;
            if( found ) array_push(props, STRDUP(line));
        }
        if(z->gs) for each_substring(z->gs, "\r\n", line) {
            const char *found = strstr(line, "/""//");
            if( found > line && line[0] == '/' && line[1] == '/' ) continue;
            if( found ) array_push(props, STRDUP(line));
        }
    } else {
        array(char*) *found = map_find( shader_reflect, parent_reflection );
        // ASSERT(found, "parent:%d", parent_reflection);
        if( found ) props = *found;
    }

    if( props ) {
        map_insert(shader_reflect, z->program, props);
        for( int i = 0; i < array_count(props); ++i ) shader_apply_param(z->program, i);
    }

    return program;
}

static
shader_t shader2_dup( const shader_t s ) {
    shader_t z = s;

    // dup strings
    z.gs = STRDUP(z.gs);
    z.vs = STRDUP(z.vs);
    z.fs = STRDUP(z.fs);
    z.attribs = STRDUP(z.attribs);
    z.fragcolors = STRDUP(z.fragcolors);
    z.cdefines = 0;

    z.constants = 0; array_dup(z.constants, s.constants);
    for( int i = 0; i < array_count(z.constants); ++i ) z.constants[i] = STRDUP(s.constants[i]);

    z.mutables = 0; array_dup(z.mutables, s.mutables);
    for( int i = 0; i < array_count(z.mutables); ++i ) z.mutables[i] = STRDUP(s.mutables[i]);

    z.dirty = ~0ull;
    z.uniforms = 0; array_dup(z.uniforms, s.uniforms);
    for( int i = 0; i < array_count(z.uniforms); ++i ) z.uniforms[i].location = -1;

    // link program to get a new id
    z.program = shader2_compile_and_link(&z, s.program);

    return z;
}

void shader2_setdefine(shader_t *s, const char *define, char digit) {
    int len = strlen(define);
    for (unsigned i = 0; i < array_count(s->mutables); i++) {
        int slen = (int)(strchr(s->mutables[i], '=') - s->mutables[i]);
        if( slen < len ) continue;
        if( !strncmp(define, s->mutables[i], len) ) { // USE_XXX=x
            if( s->mutables[i][len+1] != digit ) { // +1 to skip '='
                s->mutables[i][len+1] = digit;
                s->reload = 1; // dirty |= 1ull << i;
            }
            return;
        }
    }
}

shader_t shader2(const char *gs, const char *vs, const char *fs, const char *defines, const char *attribs, const char *fragcolors) {
    // @todo: recover .shaders/[hash]_vs/_fs/_gs from disk at this point

    // preprocess sources
    char *gspp = gs && gs[0] ? file_preprocess(gs, "shaderlib/", "shader()") : NULL;
    char *vspp = vs && vs[0] ? file_preprocess(vs, "shaderlib/", "shader()") : NULL;
    char *fspp = fs && fs[0] ? file_preprocess(fs, "shaderlib/", "shader()") : NULL;

    // copy everything
    shader_t z = {0};
    z.gs = gspp ? gspp : stringf("%s", gs ? gs : "");
    z.vs = vspp ? vspp : stringf("%s", vs ? vs : "");
    z.fs = fspp ? fspp : stringf("%s", fs ? fs : "");
    z.attribs = stringf("%s", attribs ? attribs : "");
    z.fragcolors = stringf("%s", fragcolors ? fragcolors : "");

    // hash non-dynamic variables. this hash can be used to identify the shader later on
    z.hash ^= hash_str(z.gs);
    z.hash ^= hash_str(z.vs);
    z.hash ^= hash_str(z.fs);

    // parse user #defines or mutables
    for each_substring(defines, ",", define) {
        const char *fmt = strchr(define, '=') ? "%s" : "%s=0";
        if( strbeg(define, "USE_") )
            array_push(z.mutables, stringf(fmt, define));
        else
            array_push(z.constants, stringf(fmt, define));
    }

    // infer 0/1 mutables from #defines
    for each_substring(z.gs,"\n",line) if( strstr(line,"#if USE_") /*strmatch(line, "*#*if*USE_*")*/ ) array_push(z.mutables, stringf("%s=0", strswap(strswap(va("%s",0+strstr(line, "USE_")), "\n", ""), "\r", "")));
    for each_substring(z.vs,"\n",line) if( strstr(line,"#if USE_") /*strmatch(line, "*#*if*USE_*")*/ ) array_push(z.mutables, stringf("%s=0", strswap(strswap(va("%s",0+strstr(line, "USE_")), "\n", ""), "\r", "")));
    for each_substring(z.fs,"\n",line) if( strstr(line,"#if USE_") /*strmatch(line, "*#*if*USE_*")*/ ) array_push(z.mutables, stringf("%s=0", strswap(strswap(va("%s",0+strstr(line, "USE_")), "\n", ""), "\r", "")));

    // unique sort #defines
    array_sort(z.constants, qsort_strcmpi);
    array_unique(z.constants, qsort_strcmpi); // @leak

    array_sort(z.mutables, qsort_strcmpi);
    array_unique(z.mutables, qsort_strcmpi); // @leak

    z.program = shader2_compile_and_link(&z, -1);

    // @todo: save to disk our props at this point: attribs,outcolors,defines,uniforms...
    // zip(va(".shaders.zip/%p/shader.ini",(void*)(uintptr_t)z.hash), array_inistr(props), -1);

    if( z.mutables )
    if( optioni("--shaders.precache", 1) ) {
        // do permutations here
        // @todo: save to disk our shader permutation
        // zip(va(".shaders.zip/%p/shader.%03d",(void*)(uintptr_t)hash,perm), spirv_bin, spirv_len);

        int count = array_count(z.mutables);
        if( count >= 64 ) die("shader permutations limit exceeded (%d)", count); // @fixme

        if( count >= 1 ) {
            #if 1

            array(char*) permutations = 0;

            array(char*) copy = 0;
            array(char*) original = 0;
            for(int i = 0; i < array_count(z.mutables); ++i) {
                array_push(copy, STRDUP(z.mutables[i])); // @leak
                array_push(original, STRDUP(z.mutables[i])); // @leak
            }

            int permutation = 0;

            repeat:;

            int mutables = 1 << (count);
            for (int i = 0; i < mutables; i++) {
                for(int j = 0; j < count; ++j) {
                    char *at = strchr(original[j], '=')+1;
                    int digits = strlen(at);

                    int k = count - 1 - j;
                    if( i & (1<<k) )
                    snprintf(strchr(copy[j],'=')+1, digits+1, "%*d", digits,  atoi(at));
                    else
                    snprintf(strchr(copy[j],'=')+1, digits+1, "%*d", digits, !atoi(at));
                }

                array_push(permutations, stringf("%s", strjoin(copy,",")));
            }

#if 0
            ++permutation;
            for(int j = 0; j < count; ++j) {
                char *at = strchr(original[j], '=')+1;
                int digits = strlen(at);

                if( atoi(at) > 1 ) {
                    snprintf(at, digits+1, "%*d", digits, atoi(at) / 2);
                    goto repeat;
                }
            }
#endif

            array_sort(permutations, qsort_strcmpi);
            array_unique(permutations, qsort_strcmpi);

            permutation = 0;
            for each_array(permutations, char*, it ) {
//                printf("%3d %s\n", permutation++, it);
            }

            #endif
        }
    }

    return z;
}

void shader2_destroy(shader_t *s) {
    shader_destroy(s->program);

    FREE(s->vs);
    FREE(s->fs);
    FREE(s->gs);
    FREE(s->attribs);
    FREE(s->fragcolors);
    FREE(s->cdefines);

    for each_array(s->constants, char*, str) FREE(str);
    array_free(s->constants);

    array_free(s->uniforms);

    *s = ((shader_t){0});
}


void shader2_adduniforms(shader_t* s, unsigned count, uniform_t* uniforms) {
    unsigned end = array_count(s->uniforms);
    while( count-- ) {
        uniform_t *u = uniforms + count;

        int loc = glGetUniformLocation(s->program, u->type_name + 1);
//      if( loc < 0 ) continue; // die("Error: uniform `%s` does not exist in program (%d)\n", u->name, s->program);
        u->location = loc; // refresh location in original uniform source too

        for( unsigned i = 0; i < end; ++i ) {
            if( 0 == strcmp(s->uniforms[i].type_name, u->type_name)) {
                s->uniforms[i] = *u;
                s->dirty |= 1ull << i; ASSERT(i < 64); // @fixme: +64
                i = end;
                u = NULL;
            }
        }
        if( u ) {
            s->dirty |= 1ull << array_count(s->uniforms);
            array_push(s->uniforms, *u);
        }
    }
}

void shader2_adduniform(shader_t *s, uniform_t uniform) {
    shader2_adduniforms(s, 1, &uniform);
}

// beware: will patch program if all changes are uniforms (dynamics)
// beware: will replace program if at least one of the changes is a define (mutables)
bool shader2_apply(shader_t *s) {
    int reloaded = 0;

    if( s->reload ) {
        s->reload = 0;
        shader_t n = shader2_dup(*s);
        shader2_destroy(s);
        *s = n;
        // return s->program;
        reloaded = 1;
    }

    uint64_t flags = s->dirty; s->dirty = 0;
    if( !flags ) return reloaded;

    GLint restore = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &restore);
    glUseProgram(s->program);

    for( int i = 0, end = array_count(s->uniforms); i < end; ++i ) { // @fixme: use popcnt64()
        if( !(flags & (1ull<<i)) ) continue; // @fixme: +64

        uniform_t *u = s->uniforms + i;
        uniform_set2(u, u->data ? u->data : u->any);
    }

    glUseProgram(restore);
    return reloaded;
}

#endif
