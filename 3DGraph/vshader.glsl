#version 330 core

uniform mat4 mvp_matrix;
in vec4 positionAttr;
in vec2 texCoordIn;
out vec2 texCoord;

void main()
{
    texCoord = texCoordIn;
    gl_Position = mvp_matrix * positionAttr;
}
