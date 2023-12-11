static const char *vert_shader =
    "#version 330\n"
    "in vec2 a;"
    "void main()"
    "{"
        "gl_Position=vec4(a,0,1);"
    "}";

static const int vert_len = 60;

static const char *frag_shader =
    "#version 330\n"
    "uniform vec3 t;"
    "out vec4 c;"
    "void main()"
    "{"
        "vec2 inten=abs(sin(0.1*gl_FragCoord.xy+t.z));"
        "inten *= 1.-clamp(2.*length((gl_FragCoord.xy - 0.5*t.xy)/t.y),0.0,1.0);"
        "c=vec4(inten,0,1);"
    "}";

static const int frag_len = 186;
