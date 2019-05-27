#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 uv;

uniform mat4 view, proj;
out vec3 WorldPos;
out vec3 Normal;
out vec2 UV;
out vec2 Offset;
out float Scale;

vec2 Atlas2D(float atlasIndex)
{
    return vec2(mod(atlasIndex, 8.0), floor((atlasIndex - mod(atlasIndex, 8.0)) / 8.0));
}

void main()
{
    WorldPos = position;
    Normal = normal;
    UV = uv.xy;
    Offset = 0.125 * Atlas2D(uv.z);
    Scale = 0.125 * uv.w;
    gl_Position = proj * view * vec4(WorldPos, 1.0);
} 