#version 330

uniform vec3 t;

out vec4 c;

void main() {
    vec2 inten = abs(sin(0.01 * gl_FragCoord.xy + 4.*t.z));
    inten *= 1. - clamp(2.*length((gl_FragCoord.xy - 0.5*t.xy)/t.y),0.0,1.0);
    c = vec4(0, inten,  1);
}
