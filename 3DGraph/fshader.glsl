#version 330 core

in lowp vec3 col;
out vec4 fragColor;

void main()
{
    fragColor.xyz = col;
    fragColor.w = 1.0;
}
