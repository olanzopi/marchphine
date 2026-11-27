#pragma once

inline const char* vertexShaderSource = R"( 
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;

out vec4 vertexColor;

uniform mat4 MVP;
uniform mat4 Model;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);

    vec3 transformedNormal = mat3(Model) * normal;
    float normalized = (transformedNormal.x + transformedNormal.y + transformedNormal.z) / 3;
    vertexColor = vec4(normalized, normalized, normalized, 1);
}
)";

inline const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec4 vertexColor;

void main()
{
    FragColor = vec4(vertexColor.rgb, 1);
}
)";