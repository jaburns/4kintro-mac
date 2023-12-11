#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>
#import <AVFoundation/AVFoundation.h>

#include "math.h"

#include "gfx.c"
#include "audio.c"

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

    NSWindow *window = [[NSWindow alloc]
        initWithContentRect: NSMakeRect(0, 0, 800, 600)
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

    int counter = 0;
    while (++counter < 30 + 60 * AUDIO_DURATION) {
        if (counter == 30) {
            [audioPlayer play];
        }
        if (counter > 30) {
            render();
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
