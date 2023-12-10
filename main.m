#define GL_SILENCE_DEPRECATION
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>
#import <AVFoundation/AVFoundation.h>

#include "math.h"

#define AUDIO_DURATION        3
#define AUDIO_RATE        44100
#define AUDIO_NUMCHANNELS     2
#define AUDIO_NUMSAMPLES  (AUDIO_DURATION * AUDIO_RATE * AUDIO_NUMCHANNELS)
#define WAVE_FORMAT_PCM       1
#define SIZEOF_WORD           2
static const int wav_header[11] = {
    0x46464952,
    AUDIO_NUMSAMPLES*2 + 36,
    0x45564157,
    0x20746D66,
    16,
    WAVE_FORMAT_PCM | (AUDIO_NUMCHANNELS << 16),
    AUDIO_RATE,
    AUDIO_RATE * AUDIO_NUMCHANNELS * SIZEOF_WORD,
    (AUDIO_NUMCHANNELS * SIZEOF_WORD) | ((8 * SIZEOF_WORD) << 16),
    0x61746164,
    AUDIO_NUMSAMPLES * SIZEOF_WORD
};
void run_synth(short *buffer)
{
    for (int i = 0; i < AUDIO_NUMSAMPLES; i++) {
        float amplitude = sinf((float)i/(float)AUDIO_RATE * 400.0);
        buffer[2*i+0] = (short)(amplitude*32767.0f);
        buffer[2*i+1] = (short)(amplitude*32767.0f);
    }
}

int main() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSWindow *window = [[NSWindow alloc]
        initWithContentRect: NSMakeRect(0, 0, 800, 600)
        styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
        backing: NSBackingStoreBuffered
        defer: NO
    ];

    [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];

    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    [context setView:[window contentView]];
    [context makeCurrentContext];
    [window makeKeyAndOrderFront:nil];
    [window toggleFullScreen:nil];

    NSMutableData *wavData = [NSMutableData dataWithLength:(4 * AUDIO_NUMSAMPLES + 44)];
    char *buffer = (char *)wavData.mutableBytes;
    memcpy(buffer, wav_header, 44);
    run_synth((short*)buffer + 44);

    AVAudioPlayer *audioPlayer = [[AVAudioPlayer alloc] initWithData:wavData error:(void*)(0)];
    [audioPlayer play];

    float r = 0.0, g = 0.0, b = 0.0;
    int counter = 0;
    while (++counter < 60 * 3) {
        r += 0.003;
        g += 0.007;
        b += 0.0011;
        if (r > 1.0) r -= 1.0;
        if (g > 1.0) g -= 1.0;
        if (g > 1.0) b -= 1.0;

        glClearColor(r,g,b,1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        [context flushBuffer];

        NSEvent *event;
        while ((event = [NSApp
            nextEventMatchingMask:NSEventMaskAny
            untilDate: [NSDate distantPast]
            inMode: NSDefaultRunLoopMode
            dequeue: YES
        ])) {
            [NSApp sendEvent:event];
        }

        usleep(16000);
    }

    return 0;
}
