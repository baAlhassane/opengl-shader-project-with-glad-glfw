#version 330 core
in vec2 tc;
out vec4 fragColor;

uniform sampler2D samp;

void main()
{
    fragColor = texture(samp, tc);
}
