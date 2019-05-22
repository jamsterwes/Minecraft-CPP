#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 CameraPos;
uniform vec3 LightDir;
uniform vec4 LightColor;

uniform float FogDensity;
uniform vec4 FogColor;

float calculateFog(vec3 FragPos)
{
    return exp(-pow(length(FragPos - CameraPos) * FogDensity, 2));;
}

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lighting = Albedo * 0.1;
    vec3 viewDir = normalize(CameraPos - FragPos);
    vec3 diffuse = max(dot(Normal, LightDir), 0.0) * Albedo * LightColor.rgb;
    lighting += diffuse;

    vec4 lightOutput = vec4(lighting, 1.0);

    float fogIntensity = calculateFog(FragPos);

    FragColor = mix(FogColor, lightOutput, fogIntensity);
}