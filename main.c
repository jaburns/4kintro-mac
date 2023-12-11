#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include "apple_c.h"
#include <OpenGL/gl3.h>

#include "math.h"
#include "string.h"
#include "unistd.h"

#include "audio.c"
#include "gfx.c"

#define oc_static_0(T0, this, a) \
    ((T0 (*)(Class, SEL))objc_msgSend)(this, sel_registerName(a))

#define oc_call_0(T0, T1, this, a) \
    ((T0 (*)(T1, SEL))objc_msgSend)(this, sel_registerName(a))

#define oc_static_1(T0, T1, this, a, b) \
    ((T0 (*)(Class, SEL, T1))objc_msgSend)(this, sel_registerName(a), b)

#define oc_call_1(T0, T1, T2, this, a, b) \
    ((T0 (*)(T1, SEL, T2))objc_msgSend)(this, sel_registerName(a), b)

#define oc_call_2(T0, T1, T2, T3, this, a, b, c) \
    ((T0 (*)(T1, SEL, T2, T3))objc_msgSend)(this, sel_registerName(a), b, c)



int main() {
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
    Class ns_classes[8];
    for (int i = 0; i < 8; ++i) {
        ns_classes[i] = objc_getClass(ns_class_names[i]);
    }

    id sharedApp = oc_static_0(id, ns_classes[0], "sharedApplication");
    oc_call_1(void*, id, NSInteger, sharedApp, "setActivationPolicy:", NSApplicationActivationPolicyRegular);

    id window = oc_static_0(id, ns_classes[1], "alloc");
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
    id pixelFormat = oc_static_0(id, ns_classes[2], "alloc");
    pixelFormat = oc_call_1(id, id, const NSOpenGLPixelFormatAttribute*, pixelFormat, "initWithAttributes:", attrs);

    id context = oc_static_0(id, ns_classes[3], "alloc");
    context = oc_call_2(id, id, id, id, context, "initWithFormat:shareContext:", pixelFormat, NULL);

    id contentView = oc_call_0(id, id, window, "contentView");
    oc_call_1(void, id, id, context, "setView:", contentView);
    oc_call_0(id, id, context, "makeCurrentContext");
    oc_call_1(void, id, id, window, "toggleFullScreen:", NULL);

    id wavData = oc_static_1(id, NSUInteger, ns_classes[6], "dataWithLength:", SIZEOF_WORD * AUDIO_NUMSAMPLES + 44);
    char *buffer = oc_call_0(char*, id, wavData, "mutableBytes");
    memcpy(buffer, wav_header, 44);
    run_synth((short*)(buffer + 44));

    id audioPlayer = oc_static_0(id,ns_classes[4], "alloc");
    audioPlayer = oc_call_2(id, id, id, id, audioPlayer, "initWithData:error:", wavData, NULL);

    id distantPast = oc_static_0(id,ns_classes[5], "distantPast");
    id defaultRunLoopMode = oc_static_1(id, const char*, ns_classes[7], "stringWithUTF8String:", "kCFRunLoopDefaultMode");

    int counter = 0;
    while (++counter < 30 + 60 * AUDIO_DURATION) {
        if (counter == 30) {
            oc_call_0(id, id, audioPlayer, "play");
        }
        if (counter > 30) {
            render();
        }
        oc_call_0(id, id, context, "flushBuffer");
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
                oc_call_1(void, id, id, sharedApp, "sendEvent:", event);
            } else {
                break;
            }
        }
        usleep(16000);
    }

    return 0;
}
