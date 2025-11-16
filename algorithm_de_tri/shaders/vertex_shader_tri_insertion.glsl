#version 330 core

layout(location = 0) in vec2 aPos;
uniform float uOffsetX;

void main() {
    vec2 pos = aPos;
    pos.x += uOffsetX;
    gl_Position = vec4(pos, 0.0, 1.0);
}
