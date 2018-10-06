#version 330 core

out lowp vec3 col;
in vec4 positionAttr;
in vec4 colorAttr;

void main()
{
   col = colorAttr.xyz;
   gl_Position = positionAttr;
}
