#version 330 core

out vec4 fragColor;

uniform sampler2D Atlas;
uniform int AtlasIDX;

in vec2 UV;

const int AtlasX = 4;
const int AtlasY = 4;

void main()
{
    vec2 UVOffset = vec2(float(mod(AtlasIDX, AtlasX)) / float(AtlasX), floor(float(AtlasIDX / AtlasY)) / float(AtlasY));
    fragColor = vec4(texture(Atlas, vec2(UV.x / AtlasX, UV.y / AtlasY) + UVOffset).rgb, 1.0);
}