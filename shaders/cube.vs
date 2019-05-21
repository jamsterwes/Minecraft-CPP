#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 positionOffset;
layout (location = 3) in vec2 atlasIndex;

uniform mat4 model, view, proj;
out vec2 UV;

vec2 cheatIndex(vec2 UV, vec2 IDX)
{
    vec2 temp = UV;
    temp *= 0.125;
    return temp + (IDX * 0.125);
}

void main()
{
    gl_Position = proj * view * model * vec4(position + positionOffset, 1.0);
    UV = cheatIndex(texCoord, atlasIndex);
} 