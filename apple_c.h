#include <objc/objc-runtime.h>
#include <objc/NSObjCRuntime.h>
#include <CoreFoundation/CFCGTypes.h>

#define NSApplicationActivationPolicyRegular 0

typedef CGRect NSRect;

typedef enum {
    NSBackingStoreRetained = 0,
    NSBackingStoreNonretained = 1,
    NSBackingStoreBuffered = 2,
} NSBackingStoreType;

typedef enum {
    NSWindowStyleMaskBorderless = 0,
    NSWindowStyleMaskTitled = 1 << 0,
    NSWindowStyleMaskClosable = 1 << 1,
    NSWindowStyleMaskMiniaturizable = 1 << 2,
    NSWindowStyleMaskResizable	= 1 << 3,
    NSWindowStyleMaskTexturedBackground = 1 << 8,
    NSWindowStyleMaskUnifiedTitleAndToolbar = 1 << 12,
    NSWindowStyleMaskFullScreen = 1 << 14,
    NSWindowStyleMaskFullSizeContentView = 1 << 15,
    NSWindowStyleMaskUtilityWindow = 1 << 4,
    NSWindowStyleMaskDocModalWindow = 1 << 6,
    NSWindowStyleMaskNonactivatingPanel = 1 << 7,
    NSWindowStyleMaskHUDWindow = 1 << 13
} NSWindowStyleMask;

enum {
    NSOpenGLPFAAllRenderers           =   1,	/* choose from all available renderers          */
    NSOpenGLPFATripleBuffer           =   3,	/* choose a triple buffered pixel format        */
    NSOpenGLPFADoubleBuffer           =   5,	/* choose a double buffered pixel format        */
    NSOpenGLPFAAuxBuffers             =   7,	/* number of aux buffers                        */
    NSOpenGLPFAColorSize              =   8,	/* number of color buffer bits                  */
    NSOpenGLPFAAlphaSize              =  11,	/* number of alpha component bits               */
    NSOpenGLPFADepthSize              =  12,	/* number of depth buffer bits                  */
    NSOpenGLPFAStencilSize            =  13,	/* number of stencil buffer bits                */
    NSOpenGLPFAAccumSize              =  14,	/* number of accum buffer bits                  */
    NSOpenGLPFAMinimumPolicy          =  51,	/* never choose smaller buffers than requested  */
    NSOpenGLPFAMaximumPolicy          =  52,	/* choose largest buffers of type requested     */
    NSOpenGLPFASampleBuffers          =  55,	/* number of multi sample buffers               */
    NSOpenGLPFASamples                =  56,	/* number of samples per multi sample buffer    */
    NSOpenGLPFAAuxDepthStencil        =  57,	/* each aux buffer has its own depth stencil    */
    NSOpenGLPFAColorFloat             =  58,	/* color buffers store floating point pixels    */
    NSOpenGLPFAMultisample            =  59,    /* choose multisampling                         */
    NSOpenGLPFASupersample            =  60,    /* choose supersampling                         */
    NSOpenGLPFASampleAlpha            =  61,    /* request alpha filtering                      */
    NSOpenGLPFARendererID             =  70,	/* request renderer by ID                       */
    NSOpenGLPFANoRecovery             =  72,	/* disable all failure recovery systems         */
    NSOpenGLPFAAccelerated            =  73,	/* choose a hardware accelerated renderer       */
    NSOpenGLPFAClosestPolicy          =  74,	/* choose the closest color buffer to request   */
    NSOpenGLPFABackingStore           =  76,	/* back buffer contents are valid after swap    */
    NSOpenGLPFAScreenMask             =  84,	/* bit mask of supported physical screens       */
    NSOpenGLPFAAllowOfflineRenderers  =  96,  /* allow use of offline renderers               */
    NSOpenGLPFAAcceleratedCompute     =  97,	/* choose a hardware accelerated compute device */
    NSOpenGLPFAOpenGLProfile          =  99,    /* specify an OpenGL Profile to use             */
    NSOpenGLPFAVirtualScreenCount     = 128,	/* number of virtual screens in this format     */

    NSOpenGLPFAStereo                   =   6,
    NSOpenGLPFAOffScreen                =  53,
    NSOpenGLPFAFullScreen               =  54,
    NSOpenGLPFASingleRenderer           =  71,
    NSOpenGLPFARobust                   =  75,
    NSOpenGLPFAMPSafe                   =  78,
    NSOpenGLPFAWindow                   =  80,
    NSOpenGLPFAMultiScreen              =  81,
    NSOpenGLPFACompliant                =  83,
    NSOpenGLPFAPixelBuffer              =  90,
    NSOpenGLPFARemotePixelBuffer        =  91,
};

enum {
    NSOpenGLProfileVersionLegacy      = 0x1000,   /* choose a Legacy/Pre-OpenGL 3.0 Implementation */
    NSOpenGLProfileVersion3_2Core     = 0x3200,   /* choose an OpenGL 3.2 Core Implementation      */
    NSOpenGLProfileVersion4_1Core     = 0x4100    /* choose an OpenGL 4.1 Core Implementation      */
};

NSUInteger NSEventMaskAny = 0xffffffffffffffff;

typedef uint32_t NSOpenGLPixelFormatAttribute;
