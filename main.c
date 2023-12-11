#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include "apple_c.h"
#include <OpenGL/gl3.h>

#include "math.h"
#include "string.h"
#include "unistd.h"

#include "audio.c"
#include "gfx.c"

static void* oc_call_0(BOOL getclass, void* this, const char* a) {
    if (getclass) this = objc_getClass(this);
    return ((void* (*)(void*, void*))objc_msgSend)(this, sel_registerName(a));
}
static void* oc_call_1(BOOL getclass, void* this, const char* a, void* b) {
    if (getclass) this = objc_getClass(this);
    return ((void* (*)(void*, void*, void*))objc_msgSend)(this, sel_registerName(a), b);
}
static void* oc_call_2(void* this, const char* a, void* b, void* c) {
    return ((void* (*)(void*, void*, void*, void*))objc_msgSend)(this, sel_registerName(a), b, c);
}

int main() {
    id sharedApp = oc_call_0(true, "NSApplication", "sharedApplication");
    oc_call_1(false, sharedApp, "setActivationPolicy:", NSApplicationActivationPolicyRegular);

    id window = oc_call_0(true, "NSWindow", "alloc");
    window = ((id (*)(id, SEL, NSRect, NSUInteger, NSBackingStoreType, BOOL))objc_msgSend)(
        window,
        sel_registerName("initWithContentRect:styleMask:backing:defer:"),
        (NSRect){{0.0, 0.0}, {800.0,600.0}},
        NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable,
        NSBackingStoreBuffered,
        NO
    );

    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };

    id pixelFormat = oc_call_0(true, "NSOpenGLPixelFormat", "alloc");
    pixelFormat = oc_call_1(false, pixelFormat, "initWithAttributes:", attrs);

    id context = oc_call_0(true, "NSOpenGLContext", "alloc");
    context = oc_call_2(context, "initWithFormat:shareContext:", pixelFormat, NULL);

    id contentView = oc_call_0(false, window, "contentView");
    oc_call_1(false, context, "setView:", contentView);
    oc_call_0(false, context, "makeCurrentContext");
    oc_call_1(false, window, "toggleFullScreen:", NULL);

    id wavData = oc_call_1(true, "NSMutableData", "dataWithLength:", SIZEOF_WORD * AUDIO_NUMSAMPLES + 44);
    char *buffer = oc_call_0(false, wavData, "mutableBytes");
    memcpy(buffer, wav_header, 44);
    run_synth((short*)(buffer + 44));

    id audioPlayer = oc_call_0(true, "AVAudioPlayer", "alloc");
    audioPlayer = oc_call_2(audioPlayer, "initWithData:error:", wavData, NULL);

    id distantPast = oc_call_0(true, "NSDate", "distantPast");
    id defaultRunLoopMode = oc_call_1(true, "NSString", "stringWithUTF8String:", "kCFRunLoopDefaultMode");

    int counter = 0;
    while (++counter < 30 + 60 * AUDIO_DURATION) {
        if (counter == 30) {
            oc_call_0(false, audioPlayer, "play");
        }
        if (counter > 30) {
            render();
        }
        oc_call_0(false, context, "flushBuffer");
        for (;;) {
            id event = ((id (*)(id, SEL, NSUInteger, id, id, BOOL))objc_msgSend)(
                sharedApp,
                sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"),
                NSEventMaskAny,
                distantPast,
                defaultRunLoopMode,
                YES
            );
            if (event) {
                oc_call_1(false, sharedApp, "sendEvent:", event);
            } else {
                break;
            }
        }
        usleep(16000);
    }

    return 0;
}
