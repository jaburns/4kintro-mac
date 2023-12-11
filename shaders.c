static const char *vert_shader =
    "#version 330\n"
    "in vec2 a;"
    "void main()"
    "{"
        "gl_Position=vec4(a,0,1);"
    "}";

static const char *frag_shader =
    "#version 330\n"
    "uniform float t;"
    "out vec4 c;"
    "void main()"
    "{"
        "vec2 inten=sin(0.1*gl_FragCoord.xy+t);"
        "c=vec4(inten,0,1);"
    "}";
