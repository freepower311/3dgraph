#version 330 core

in lowp vec3 col;
out vec4 fragColor;
uniform sampler2D texture;

in vec2 texCoord;

void main()
{
    //fragColor.xyz = col;
    //fragColor.w = 1.0;
    fragColor = texture2D(texture, texCoord);
}
