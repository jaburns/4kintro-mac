#version 330

uniform vec3 T;
out vec4 C;


#define ITERATIONS  100
#define EPSILON      0.0001
#define MAX_DIST    100.
#define PI 3.14159265358979

mat2 rot(float theta)
{
    float c = cos(theta);
    float s = sin(theta);
	return mat2(c, s, -s, c);
}

float sdBox2d(vec2 p, float r)
{
    vec2 q = abs(p) - r;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}

float sdBox( vec3 p, float b )
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdCross(vec3 p, float r)
{
    return min(sdBox2d(p.yz, r), min(sdBox2d(p.xy, r), sdBox2d(p.xz, r)));
}

float opSubtract(float a, float b)
{
    return max(a, -b);
}

float map(vec3 p, float scale)
{
    p /= scale;

    p = mod(p + 3., 6.) - 3.;

    float d = sdBox(p, 3.);
    d = opSubtract(d, sdCross(p, 1.));

    float s = 1.;
    for (int i = 0; i < 6; ++i) {
        vec3 p1 = mod(s*p + 1., 2.) - 1.;
        float cutDist = sdCross(p1, 1. / 3.) / s;
        d = opSubtract(d, cutDist);
        s *= 3.;
    }

    return d * scale;
}

struct MarchResult
{
    vec3 pos;
    float dist;
    float ao;
};

MarchResult march(vec3 ro, vec3 rd, float scale)
{
    float dist, totalDist = 0.0;

    int i;
    for (i = 0; i < ITERATIONS; ++i) {
    	dist = map(ro, scale);
        if (dist < EPSILON || totalDist > MAX_DIST) break;
        totalDist += dist;
        ro += rd * dist;
    }

    return MarchResult(ro, dist < EPSILON ? totalDist : -1., 1. - float(i)/100.);
}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 iResolution = T.xy;
    float iTime = T.z;

    vec2 uv = fragCoord/iResolution.yy - vec2(.5*iResolution.x/iResolution.y, .5);

    // ===== Camera =====

    float T = 20.;
    float t = mod(iTime / T, 1.);
    float zoom = mix(1., 3., t);
	vec3 rd;
    const float x0 = 2.;
    const float xx = 6.;
    const float yy = 8. - (.666);

    vec3 ro;
    if (t < .5) {
        t *= 2.;
        ro = zoom*vec3(x0,0, -yy * (  2.*t-t*t ));
        float angle = -PI / 2. * (.5-.5*cos(PI*t));
        rd = normalize(vec3(uv, -1));
        rd.xz *= rot(angle);
        rd.yz *= rot(2.*PI* (.5-.5*cos(PI*t)) );
    } else {
        t = 2.*(t - .5);
        vec3 roA = zoom*vec3(x0,0,-yy);
        vec3 roB = zoom*vec3(x0+xx*t,0,-yy);
        ro = mix(roA, roB, smoothstep(0.,1.,clamp(4.*t, 0., 1.)));
        float look = (1.-(2.*t-1.)*(2.*t-1.)) * sin(2.*PI*t);
        rd = normalize(vec3(uv + .5*look, -1));
        rd.xz *= rot(-PI / 2.);
        rd.zy *= rot(.25*look);
    }

    // ==================

    MarchResult m = march(ro, rd, zoom);

    float vis = 0.;
    if (m.dist >= 0.) {
    	vis = exp(-.2 * m.dist) * m.ao;
    }

    int step = int(floor(iTime * 2.0)) % 4;
    vec3 DARK, LIGHT;

    if (step == 0) {
        DARK = mix(vec3(97., 8., 52.) / 255., vec3(0), .2);
        LIGHT = vec3(255., 165., 0.) / 255.;
    } else if (step == 1) {
        DARK = mix(vec3(8., 52., 97.) / 255., vec3(0), .2);
        LIGHT = vec3(165., 0., 255.) / 255.;
    } else if (step == 2) {
        DARK = mix(vec3(52., 97., 8.) / 255., vec3(0), .2);
        LIGHT = vec3(0., 255., 165.) / 255.;
    } else {
        DARK = mix(vec3(97., 52., 8.) / 255., vec3(0), .2);
        LIGHT = vec3(255., 0., 165.) / 255.;
    }

    C = vec4(mix(DARK, LIGHT, vis), 0);
}

