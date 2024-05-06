#pragma once

constexpr auto vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

void main(void)
{
    gl_Position = vec4(aPos, 1.0);
    texCoord = aTexCoord;
}
)";

constexpr auto fragmentShaderSource = R"(
#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D tex;
uniform int time;

vec3 rainbow(float deg)
{
    float c = mod(deg, 60) / 60;
    float r, g, b;

    int stage = int(deg / 60);
    switch (stage) {
        case 0:
            r = 1;
            g = c;
            b = 0;
            break;
        case 1:
            r = 1 - c;
            g = 1;
            b = 0;
            break;
        case 2:
            r = 0;
            g = 1;
            b = c;
            break;
        case 3:
            r = 0;
            g = 1 - c;
            b = 1;
            break;
        case 4:
            r = c;
            g = 0;
            b = 1;
            break;
        case 5:
            r = 1;
            g = 0;
            b = 1 - c;
            break;
    }

    return vec3(r, g, b);
}

void main(void)
{
    vec3 col = rainbow(mod(time / 100, 360));
    outColor = texture(tex, texCoord) * vec4(col, 1);
}
)";
