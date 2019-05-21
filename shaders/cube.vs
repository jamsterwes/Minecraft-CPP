#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 model, view, proj;
out vec2 UV;

void main()
{
    gl_Position = proj * view * model * vec4(position, 1.0);
    UV = texCoord;
} 