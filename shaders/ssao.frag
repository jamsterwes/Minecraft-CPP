#version 430 core

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 Samples[64];

uniform int kernelSize;
uniform float radius;
uniform float bias;

uniform vec2 ScreenSize;

uniform mat4 proj, view;

void main()
{
    vec2 noiseScale = ScreenSize / 4.0;

    vec3 fragPosWS = texture(gPosition, TexCoords).xyz;
    vec3 normalWS = normalize(texture(gNormal, TexCoords).rgb);

    vec3 fragPosVS = vec3(view * vec4(fragPosWS, 1.0));
    vec3 normalVS = normalize(mat3(view) * normalWS);

    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normalVS * dot(randomVec, normalVS));
    vec3 bitangent = cross(normalVS, tangent);
    mat3 TBN = mat3(tangent, bitangent, normalVS);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; i++)
    {
        vec3 kernSample = TBN * Samples[i];
        kernSample = fragPosVS + kernSample * radius;
        vec4 offsetUV = vec4(kernSample, 1.0);
        offsetUV = proj * offsetUV;    // from view to clip-space
        offsetUV.xy /= offsetUV.w;               // perspective divide
        offsetUV.xy = offsetUV.xy * 0.5 + 0.5; // transform to range 0.0 - 1.0  

        float sampleDepth = vec3(view * vec4(texture(gPosition, offsetUV.xy).rgb, 1.0)).z;

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPosVS.z - sampleDepth));
        occlusion += (sampleDepth >= kernSample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}