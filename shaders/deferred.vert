#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 5) in float texCoordOffset;
layout (location = 3) in vec3 positionOffset;
layout (location = 4) in vec2 atlasIndex;
layout (location = 6) in float dimension;

uniform mat4 view, proj;
out vec3 WorldPos;
out vec3 Normal;
out vec2 UV;
out vec2 Offset;
out float Scale;

void main()
{
    WorldPos = (position * dimension) + positionOffset;
    Normal = normal;
    UV = texCoord;
    Offset = 0.125 * (atlasIndex + vec2(texCoordOffset, 0));
    Scale = 0.125 * dimension;
    gl_Position = proj * view * vec4(WorldPos, 1.0);
} 