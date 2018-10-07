#version 330 core

in vec4 positionAttr;
in vec4 colorAttr;
out lowp vec3 col;

void main()
{
    col = colorAttr.xyz;
    gl_Position = positionAttr;
}
