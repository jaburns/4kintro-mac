#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>
#import <AVFoundation/AVFoundation.h>
#include "math.h"

#include "audio.c"
#include "shaders.gen.c"

static const NSOpenGLPixelFormatAttribute attrs[] = {
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
    NSOpenGLPFAColorSize, 24,
    NSOpenGLPFAAlphaSize, 8,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    0
};

int main() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSRect screen_rect = [[NSScreen mainScreen] frame];
    float scale = [[NSScreen mainScreen] backingScaleFactor];
    float width = (float)screen_rect.size.width * scale;
    float height = (float)screen_rect.size.height * scale;

    NSWindow *window = [[NSWindow alloc]
        initWithContentRect: screen_rect
        styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
        backing: NSBackingStoreBuffered
        defer: NO
    ];

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    [context setView:[window contentView]];
    [context makeCurrentContext];
    [window toggleFullScreen:nil];

    NSMutableData *wavData = [NSMutableData dataWithLength:(SIZEOF_WORD * AUDIO_NUMSAMPLES + 44)];
    char *buffer = (char *)wavData.mutableBytes;
    memcpy(buffer, wav_header, 44);
    run_synth((short*)buffer + 44);

    AVAudioPlayer *audioPlayer = [[AVAudioPlayer alloc] initWithData:wavData error:(void*)(0)];

// --- gfx init -----------------------------------------------------

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    int slen = VERT_MIN_LENGTH;
    glShaderSource(vert, 1, &VERT_MIN, &slen);
    glCompileShader(vert);

#ifdef J_DEBUG
    GLint isCompiled = 0;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE) {
        GLsizei maxLength = 2048;
        GLchar errorLog[2048];
        glGetShaderInfoLog(vert, maxLength, &maxLength, &errorLog[0]);
        printf("%s", errorLog);
        exit(1);
    }
#endif

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    slen = FRAG_MIN_LENGTH;
    glShaderSource(frag, 1, &FRAG_MIN, &slen);
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
    static const char bufdata[6] = { 1, 1, 1, 128, 128, 1 };
    glGenBuffers(1, &tri_vertex_buff);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_buff);
    glBufferData(GL_ARRAY_BUFFER, 6, bufdata, GL_STATIC_DRAW);

// ------------------------------------------------------------------

    int counter = 0;
    while (++counter < 30 + 60 * AUDIO_DURATION) {
        if (counter == 30) {
            [audioPlayer play];
            glViewport(0,0,width,height);
        }
        if (counter > 30) {
// ------------------------------------------------------------------
            glBindVertexArray(vao);
            glUseProgram(program);
            GLint posLoc = glGetAttribLocation(program, "a");
            glBindBuffer(GL_ARRAY_BUFFER, tri_vertex_buff);
            glUniform3f(uniLoc, width, height, (float)counter/60.0);
            glEnableVertexAttribArray(posLoc);
            glVertexAttribPointer(posLoc, 2, GL_BYTE, false, 0, 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
// ------------------------------------------------------------------
        }
        [context flushBuffer];
        NSEvent *event;
        for (;;) {
            event = [NSApp
                nextEventMatchingMask: NSEventMaskAny
                untilDate: [NSDate distantPast]
                inMode: NSDefaultRunLoopMode
                dequeue: YES
            ];
            if (event) {
                [NSApp sendEvent:event];
            } else {
                break;
            }
        }
        usleep(16000);
    }

    return 0;
}
