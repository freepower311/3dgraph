#version 330 core

uniform mat4 mvp_matrix;
in vec4 positionAttr;
in vec4 colorAttr;
in vec2 texCoordIn;
out lowp vec3 col;
out vec2 texCoord;

void main()
{
    texCoord = texCoordIn;
    col = colorAttr.xyz;
    gl_Position = mvp_matrix * positionAttr;
}
