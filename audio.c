#define AUDIO_DURATION        2
#define AUDIO_RATE        44100
#define AUDIO_NUMCHANNELS     1
#define AUDIO_NUMSAMPLES  (AUDIO_DURATION * AUDIO_RATE * AUDIO_NUMCHANNELS)
#define SIZEOF_WORD           2

static const int wav_header[11] = {
    0x46464952,
    AUDIO_NUMSAMPLES*SIZEOF_WORD + 36,
    0x45564157,
    0x20746D66,
    16,
    1 | (AUDIO_NUMCHANNELS << 16),
    AUDIO_RATE,
    AUDIO_RATE * AUDIO_NUMCHANNELS * SIZEOF_WORD,
    (AUDIO_NUMCHANNELS * SIZEOF_WORD) | ((8 * SIZEOF_WORD) << 16),
    0x61746164,
    AUDIO_NUMSAMPLES * SIZEOF_WORD
};

//===== Math utility functions =================================================

static int f2i( float x )
{
    return (int)x;
}

static float pow_( float a, float b )
{
    return powf( a, b );
}

static float floor_( float x )
{
    return f2i( x - 0.5f );
}

static float clamp(float x, float lowerlimit, float upperlimit)
{
    if (x < lowerlimit) x = lowerlimit;
    if (x > upperlimit) x = upperlimit;
    return x;
}

static float smoothstep(float edge0, float edge1, float x)
{
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3 - 2 * x);
}

static float mix(float a, float b, float t)
{
    return a + t*(b - a);
}

static float fract(float a)
{
    return a - floor_(a);
}

static float mod(float a, float b)
{
    return a - b * floor_( a / b );
}

//==============================================================================

static float hash( float i )
{
    return fract( sin( i * 12.9898f ) * 43758.5453f );
}

static float funcRand( float x )
{
    float a = hash( floor_( x ));
    float b = hash( floor_( x + 1.0f ));
    return 1.0f - 2.0f * mix( a, b, smoothstep( 0.0f, 1.0f, fract( x )));
}

static float taylorSquareWave( float x )
{
    float result = 0.0f;

    for( int i = 1; i <= 5; i += 2 )
    {
        float n = (float)i;
        result += 4.0f / 3.14159f / n * sin( n * x );
    }

    return result;
}

static float kick( float time )
{
    float attack = clamp( 400.0f*time, 0.0f, 1.0f );
    float decay = 1. - smoothstep( 0.4f, 0.5f, time );
    return attack * decay * sin( 220.0f * pow_( time, 0.65f ));
}

static float hat( float time )
{
     return 0.33f * funcRand( 20000.0f * pow_( 2.72f, -10.0f*time )) * pow_( 2.72f, -30.0f*time );
}

static float latestKickStartTime( float t )
{
    return floor_( t * 2.0f ) / 2.0f;
}

static float latestHatStartTime( float t )
{
    if (t > 3.75f) return 3.75f;
    return floor_( t + 0.5f ) - 0.5f;
}

static float getSound( float time )
{
    const float FADE_LEN = 8.0f;

    float t = mod( time, 4. );

    float sineRamp = clamp((time - 4.0f) / 12.0f, 0.0f, 1.0f);
    float sqrRamp  = clamp((time - 8.0f) /  8.0f, 0.0f, 1.0f);

    const float padF = 32.0f;
    const float volume = 1.0; // clamp(time / FADE_LEN, 0.0f, 1.0f) * clamp(((float)AUDIO_DURATION - time) / FADE_LEN, 0.0f, 1.0f);

    float signal =
        1.00f * kick( t - latestKickStartTime( t )) +
        0.50f * hat( t - latestHatStartTime( t )) +
        0.25f * taylorSquareWave( 2.0f * 3.14159f * (padF + 2.0f) * time ) +
        0.50f * sin( 4.0f * 3.14159f * padF * time );

    return volume * clamp( signal, -1.0f, 1.0f );
}

static void run_synth(short *buffer)
{
    for (int i = 0; i < AUDIO_NUMSAMPLES / AUDIO_NUMCHANNELS; i++) {
        float amplitude = getSound((float)i/(float)AUDIO_RATE);
        //float amplitude = sinf((float)i/(float)AUDIO_RATE * 400.0 * 2.0 * 3.14159);
        buffer[i] = (short)(amplitude*32767.0f);
    }
}

