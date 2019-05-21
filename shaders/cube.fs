#version 330 core

out vec4 fragColor;

uniform highp sampler2D Atlas;
in vec2 UV;

void main()
{
    vec4 block = vec4(texture(Atlas, UV).rgb, 1.0);
    fragColor = block;
}