#version 330 core

varying lowp vec3 col;
layout(location = 0) in vec4 positionAttr;
layout(location = 1) in vec4 colorAttr;

void main()
{
   col = colorAttr.xyz;
   gl_Position = positionAttr;
}
