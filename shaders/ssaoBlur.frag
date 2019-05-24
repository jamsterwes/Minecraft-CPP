#version 430 core

out float aoBlur;

uniform int blurAmount;
uniform sampler2D ao;
in vec2 TexCoords;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ao, 0));
    float result = 0.0;
    for (int x = -blurAmount; x < blurAmount; ++x) 
    {
        for (int y = -blurAmount; y < blurAmount; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ao, TexCoords + offset).r;
        }
    }
    aoBlur = result / float(pow(blurAmount, 4));
}