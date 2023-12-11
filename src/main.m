#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

//#define J_DEBUG

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>
#import <AVFoundation/AVFoundation.h>
#include <mach/mach_time.h>
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

#ifdef J_DEBUG
    GLsizei errorMaxLen = 32768;
    GLchar errorLog[32768];

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    int slen = VERT_MIN_LENGTH;
    glShaderSource(vert, 1, &VERT_MIN, &slen);
    glCompileShader(vert);

    glGetShaderInfoLog(vert, errorMaxLen, &errorMaxLen, &errorLog[0]);
    if (strlen(errorLog) > 0) {
        printf("VERT SHADER ERROR:\n%s", errorLog);
        exit(1);
    }

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    slen = FRAG_MIN_LENGTH;
    glShaderSource(frag, 1, &FRAG_MIN, &slen);
    glCompileShader(frag);

    glGetShaderInfoLog(frag, errorMaxLen, &errorMaxLen, &errorLog[0]);
    if (strlen(errorLog) > 0) {
        printf("FRAG SHADER ERROR:\n%s", errorLog);
        exit(1);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glGetProgramInfoLog(program, errorMaxLen, &errorMaxLen, &errorLog[0]);
    if (strlen(errorLog) > 0) {
        printf("SHADER LINK ERROR:\n%s", errorLog);
        exit(1);
    }
#else
    GLuint program = glCreateProgram();
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    int slen = VERT_MIN_LENGTH;
    glShaderSource(vert, 1, &VERT_MIN, &slen);
    slen = FRAG_MIN_LENGTH;
    glShaderSource(frag, 1, &FRAG_MIN, &slen);
    glCompileShader(vert);
    glCompileShader(frag);
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
#endif

    static const char bufdata[6] = { 1, 1, 1, 128, 128, 1 };

    GLint posLoc = glGetAttribLocation(program, "a");
    GLint uniLoc = glGetUniformLocation(program, "t");
    glUseProgram(program);

    GLuint verts, vao;

    glGenBuffers(1, &verts);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, verts);
    glBufferData(GL_ARRAY_BUFFER, 6, bufdata, GL_STATIC_DRAW);
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_BYTE, false, 0, 0);
    glViewport(0, 0, width, height);

    mach_timebase_info_data_t info;
    mach_timebase_info(&info);

    [audioPlayer play];
    uint64_t start = mach_absolute_time();

    for (;;) {
        uint64_t time = mach_absolute_time();
        uint64_t total_nano = (time - start) * info.numer / info.denom;
        float secs = (float)(total_nano / 100000) / 10000.0f;
        if (secs > (float)AUDIO_DURATION) {
            break;
        }
        glUniform3f(uniLoc, width, height, secs);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        [context flushBuffer];
        NSEvent *event;
        while ((event = [NSApp
            nextEventMatchingMask: NSEventMaskAny
            untilDate: [NSDate distantPast]
            inMode: NSDefaultRunLoopMode
            dequeue: YES
        ])) {
            [NSApp sendEvent:event];
        }
        usleep(1);
    }

    return 0;
}
