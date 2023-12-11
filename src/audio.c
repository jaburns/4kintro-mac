#define AUDIO_DURATION        4
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

static float kick( float time )
{
    float attack = clamp( 400.0f*time, 0.0f, 1.0f );
    float decay = 1. - smoothstep( 0.4f, 0.5f, time );
    return attack * decay * sin( 220.0f * powf( time, 0.65f ));
}

static float getSound( float time )
{
    if (time < 0.5) return 0.0;
    time -= 0.5;
    return clamp(kick(fmodf(time, .5)), -1.0, 1.0);
}

static void run_synth(short *buffer)
{
    for (int i = 0; i < AUDIO_NUMSAMPLES / AUDIO_NUMCHANNELS; i++) {
        float amplitude = getSound((float)i/(float)AUDIO_RATE);
        buffer[i] = (short)(amplitude*32767.0f);
    }
}

