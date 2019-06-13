#version 430 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 UV;
in vec2 Offset;
in float Scale;
in vec3 WorldPos;
in vec3 Normal;

uniform bool OctreeDebug;

uniform sampler2D Atlas;

vec2 Unpack_UV_Offset_Scale()
{
    return mod(UV * Scale, 0.125) + Offset;
}

void main()
{
    vec4 atlasTex = texture(Atlas, Unpack_UV_Offset_Scale());
    gPosition = vec4(WorldPos, 0.0);
    gNormal = vec4(normalize(Normal), 1.0);
    if (OctreeDebug) gAlbedoSpec.rgb = vec3(log2(Scale * 8) * 0.125);
    else gAlbedoSpec.rgb = atlasTex.rgb;
    gAlbedoSpec.a = 0.05;

    if (atlasTex.a < 0.05)
        discard;
}