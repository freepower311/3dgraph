#version 330 core

uniform mat4 mvpMatrix;
uniform mat4 modelViewMatrix;
in vec4 positionAttr;
in vec2 texCoordIn;
in vec3 normalsIn;
out vec2 texCoord;

void main()
{
    texCoord = texCoordIn;
    gl_Position = mvpMatrix * positionAttr;
}
