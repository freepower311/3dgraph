#version 130

out vec4 fragColor;

void main()
{
    fragColor = vec4(gl_FragCoord.z);
}