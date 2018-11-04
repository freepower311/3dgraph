#version 130

uniform mat4 mvpMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;
uniform mat4 depthMVP;
in vec4 positionAttr;
in vec2 texCoordIn;
in vec3 normalsIn;
out vec2 texCoord;
out vec4 viewSpaceNormal;
out vec4 viewSpacePosition;
out vec4 shadowMapCoord;

void main()
{
    texCoord = texCoordIn;
    viewSpaceNormal = normalMatrix * vec4(normalsIn,0.0);
    viewSpacePosition = modelViewMatrix * positionAttr;
    gl_Position = mvpMatrix * positionAttr;
    shadowMapCoord = depthMVP * viewSpacePosition;
    shadowMapCoord.w = 1.0;
    shadowMapCoord.xyz *= vec3(0.5, 0.5, 0.5);
    shadowMapCoord.xyz += shadowMapCoord.w * vec3(0.5, 0.5, 0.5);
}
