#version 330 core

out vec4 fragColor;

uniform highp sampler2D Atlas;
in vec2 UV;
in vec3 WorldPos;
in vec3 Normal;
uniform vec3 CameraPos;
uniform vec3 ViewDir;
uniform float FogDensity;
uniform vec4 FogColor;

uniform vec3 LightDir;
uniform vec4 LightColor;
uniform vec4 AmbientColor;
uniform float SpecularStrength;

float calculateFog()
{
    return exp(-pow(length(WorldPos - CameraPos) * FogDensity, 2));;
}

vec3 calculateDiffuse(vec3 objectColor)
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-LightDir);
    float diff = max(dot(norm, lightDir), 0.0);
    return diff * LightColor.rgb;
}

vec4 Lighting(vec4 inColor)
{
    vec3 diffuse = calculateDiffuse(inColor.rgb);
    return vec4((diffuse + AmbientColor.rgb) * inColor.rgb, 1.0);
}

void main()
{
    vec4 blockT = vec4(texture(Atlas, UV).rgb, 1.0);

    // if (blockT.a < 0.1)
        // discard;

    // LIGHTING PASS
    vec4 block = Lighting(blockT);
    // FOG PASS
    block = mix(FogColor, block, calculateFog());
    fragColor = vec4(block.rgb, texture(Atlas, UV).a);
}