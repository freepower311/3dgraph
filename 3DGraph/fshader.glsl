#version 330 core

uniform sampler2D texture;
in vec2 texCoord;
out vec4 fragColor;

void main()
{
    fragColor = texture2D(texture, texCoord);
}
