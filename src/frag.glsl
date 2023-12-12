#version 330

uniform vec3 t;

out vec4 c;

void main() {
    vec2 v = gl_FragCoord.xy;
    vec2 inten = abs(sin(0.01 * v+ 4.*t.z));
    inten *= 1. - clamp(2.*length((v - 0.5*t.xy)/t.y),0.0,1.0);
    c = vec4(pow(1.-fract(2. * t.z),3.), inten,  1);
}
