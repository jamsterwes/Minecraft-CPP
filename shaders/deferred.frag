#version 430 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 UV;
in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D Atlas;

void main()
{
    gPosition = vec4(WorldPos, 0.0);
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = texture(Atlas, UV).rgb;
    gAlbedoSpec.a = 0.05;
}