#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define J_DEBUG 0

#include "apple_c.h"
#include <OpenGL/gl3.h>

#include "math.h"
#include "string.h"
#include "unistd.h"

#include "audio.c"
#include "gfx.c"
#include "shaders.c"

#ifdef J_DEBUG
#  include "stdio.h"
#endif

#define CHECK_GL_ERROR() printf("\nERR: %d", glGetError())

#define oc_static_0(T0, this, a) \
    ((T0 (*)(Class, SEL))objc_msgSend)(this, a)

#define oc_call_0(T0, T1, this, a) \
    ((T0 (*)(T1, SEL))objc_msgSend)(this, a)

#define oc_static_1(T0, T1, this, a, b) \
    ((T0 (*)(Class, SEL, T1))objc_msgSend)(this, a, b)

#define oc_call_1(T0, T1, T2, this, a, b) \
    ((T0 (*)(T1, SEL, T2))objc_msgSend)(this, a, b)

#define oc_call_2(T0, T1, T2, T3, this, a, b, c) \
    ((T0 (*)(T1, SEL, T2, T3))objc_msgSend)(this, a, b, c)


static NSOpenGLPixelFormatAttribute attrs[] = {
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
    NSOpenGLPFAColorSize, 24,
    NSOpenGLPFAAlphaSize, 8,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    0
};
static const char *ns_class_names[8] = {
    "NSApplication",
    "NSWindow",
    "NSOpenGLPixelFormat",
    "NSOpenGLContext",
    "AVAudioPlayer",
    "NSDate",
    "NSMutableData",
    "NSString",
};
static const char *ns_sel_names[19] = {
    "sharedApplication",
    "setActivationPolicy:",
    "alloc",
    "initWithAttributes:",
    "initWithFormat:shareContext:",
    "contentView",
    "setView:",
    "makeCurrentContext",
    "toggleFullScreen:",
    "dataWithLength:",
    "mutableBytes",
    "initWithData:error:",
    "distantPast",
    "stringWithUTF8String:",
    "play",
    "flushBuffer",
    "nextEventMatchingMask:untilDate:inMode:dequeue:",
    "sendEvent:",
    "initWithContentRect:styleMask:backing:defer:"
};

static Class ns_classes[8];
static SEL ns_sels[19];
static NSRect rect = (NSRect){
    {0.0, 0.0},
    {800.0,600.0}
};

int main() {
    for (int i = 0; i < 8; ++i) {
        ns_classes[i] = objc_getClass(ns_class_names[i]);
    }
    for (int i = 0; i < 19; ++i) {
        ns_sels[i] = sel_registerName(ns_sel_names[i]);
    }

    id sharedApp = oc_static_0(id, ns_classes[0], ns_sels[0]);
    oc_call_1(void*, id, NSInteger, sharedApp, ns_sels[1], NSApplicationActivationPolicyRegular);

    id window = oc_static_0(id, ns_classes[1], ns_sels[2] );
    window = ((id (*)(id, SEL, NSRect, NSUInteger, NSBackingStoreType, BOOL))objc_msgSend)(
        window,
        ns_sels[18],
        rect,
        NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable,
        NSBackingStoreBuffered,
        NO
    );

    id pixelFormat = oc_static_0(id, ns_classes[2], ns_sels[2]);
    pixelFormat = oc_call_1(id, id, const NSOpenGLPixelFormatAttribute*, pixelFormat, ns_sels[3], attrs);

    id context = oc_static_0(id, ns_classes[3], ns_sels[2]);
    context = oc_call_2(id, id, id, id, context, ns_sels[4], pixelFormat, NULL);

    id contentView = oc_call_0(id, id, window, ns_sels[5]);
    oc_call_1(void, id, id, context, ns_sels[6], contentView);
    oc_call_0(id, id, context, ns_sels[7]);
    oc_call_1(void, id, id, window, ns_sels[8], NULL);

    id wavData = oc_static_1(id, NSUInteger, ns_classes[6], ns_sels[9], SIZEOF_WORD * AUDIO_NUMSAMPLES + 44);
    char *buffer = oc_call_0(char*, id, wavData, ns_sels[10]);
    memcpy(buffer, wav_header, 44);
    run_synth((short*)(buffer + 44));

    id audioPlayer = oc_static_0(id,ns_classes[4], ns_sels[2]);
    audioPlayer = oc_call_2(id, id, id, id, audioPlayer, ns_sels[11], wavData, NULL);

    id distantPast = oc_static_0(id,ns_classes[5], ns_sels[12]);
    id defaultRunLoopMode = oc_static_1(id, const char*, ns_classes[7], ns_sels[13], "kCFRunLoopDefaultMode");

// --- gfx init -----------------------------------------------------

    GLint len = strlen(vert_shader);
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_shader, &len);
    glCompileShader(vert);

#ifdef J_DEBUG
    GLint isCompiled = 0;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) {
        GLsizei maxLength = 2048;
        GLchar errorLog[2048];
        glGetShaderInfoLog(vert, maxLength, &maxLength, &errorLog[0]);
        printf("%s\n%s", vert_shader, errorLog);
        exit(1);
    }
#endif

    len = strlen(frag_shader);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_shader, &len);
    glCompileShader(frag);

#ifdef J_DEBUG
    glGetShaderiv(frag, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) {
        GLsizei maxLength = 2048;
        GLchar errorLog[2048];
        glGetShaderInfoLog(frag, maxLength, &maxLength, &errorLog[0]);
        printf("%s", errorLog);
        exit(1);
    }
#endif

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    GLint posLoc = glGetAttribLocation(program, "a");
    GLint uniLoc = glGetUniformLocation(program, "t");

#ifdef J_DEBUG
    GLsizei maxLength = 2048;
    GLchar errorLog[2048];
    glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
    printf("%s", errorLog);
#endif

    GLuint tri_vertex_buff;
    char bufdata[6] = { 1, 1, 1, 128, 128, 1 };
    glGenBuffers(1, &tri_vertex_buff);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_buff);
    glBufferData(GL_ARRAY_BUFFER, 6, bufdata, GL_STATIC_DRAW);

// ------------------------------------------------------------------

    id event;
    int counter = 0;
    while (++counter < 30 + 60 * AUDIO_DURATION) {
        if (counter == 30) {
            oc_call_0(id, id, audioPlayer, ns_sels[14]);
        }
        if (counter > 30) {
// ------------------------------------------------------------------
            glBindVertexArray(vao);
            glUseProgram(program);
            GLint posLoc = glGetAttribLocation(program, "a");
            glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_buff);
            glUniform1f(uniLoc, (float)counter);
            glEnableVertexAttribArray(posLoc);
            glVertexAttribPointer(posLoc, 2, GL_BYTE, false, 0, 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
// ------------------------------------------------------------------
        }
        oc_call_0(id, id, context, ns_sels[15]);
        while ((event = ((id (*)(id, SEL, NSUInteger, id, id, BOOL))objc_msgSend)(
            sharedApp,
            ns_sels[16],
            NSEventMaskAny,
            distantPast,
            defaultRunLoopMode,
            YES
        ))) {
            oc_call_1(void, id, id, sharedApp, ns_sels[17], event);
        }
        usleep(16000);
    }

    return 0;
}
