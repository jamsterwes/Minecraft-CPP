#version 330 core

out vec4 fragColor;

uniform sampler2D Atlas;
uniform int AtlasIDX, BreakIDX;

in vec2 UV;

const int AtlasX = 8;
const int AtlasY = 8;

vec2 GetUVOffset(int IDX)
{
    return vec2(float(mod(IDX, AtlasX)) / float(AtlasX), float(int(float(IDX) / float(AtlasY))) / float(AtlasY));
}

void main()
{
    vec4 block = vec4(texture(Atlas, vec2(UV.x / AtlasX, UV.y / AtlasY) + GetUVOffset(AtlasIDX)).rgb, 1.0);
    vec4 breakAnim = texture(Atlas, vec2(UV.x / AtlasX, UV.y / AtlasY) + GetUVOffset(56 + BreakIDX));
    fragColor = mix(block, breakAnim, breakAnim.a);
}