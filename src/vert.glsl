#version 330

in vec2 a;
void main() {
    gl_Position = vec4(a, 0, 1);
}
