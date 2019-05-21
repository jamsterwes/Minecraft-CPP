#version 330 core

out vec4 fragColor;

uniform highp sampler2D Atlas;
in vec2 UV;
in vec3 WorldPos;
uniform vec3 CameraPos;
uniform float FogDensity;
uniform vec4 FogColor;

void main()
{
    vec4 block = vec4(texture(Atlas, UV).rgb, 1.0);
    float dist = length(WorldPos - CameraPos);
    float fogAmount = exp(-pow(dist * FogDensity, 2));
    fragColor = mix(FogColor, block, fogAmount);
    // dist = log(dist) * 0.1;
    // fragColor = vec4(dist, dist, dist, 0.0);
}