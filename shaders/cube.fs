#version 330 core

out vec4 fragColor;

uniform sampler2D Atlas;
uniform vec2 AtlasIDX;
uniform int BreakIDX;

in vec2 UV;

const float ATLAS_DIM = 8;
const float ATLAS_TEX_SIZE = 1.0 / ATLAS_DIM;
const float PIXEL_SIZE = 1.0 / (ATLAS_DIM * 16.0);

vec2 index(vec2 UV, vec2 IDX)
{
    float xMin = (IDX.x * ATLAS_TEX_SIZE);
    float yMin = (IDX.y * ATLAS_TEX_SIZE);
    float xMax = (xMin + ATLAS_TEX_SIZE);
    float yMax = (yMin + ATLAS_TEX_SIZE);

    return (UV * vec2(xMax - xMin, yMax - yMin)) + vec2(xMin, yMin);
}

void main()
{
    vec4 block = vec4(texture(Atlas, index(UV, AtlasIDX)).rgb, 1.0);
    vec4 breakAnim = vec4(0.0);
    fragColor = mix(block, breakAnim, breakAnim.a);
}