#version 130

in vec4 positionAttr;
uniform mat4 depthMVP;

void main()
{
    gl_Position =  depthMVP * positionAttr;
}
