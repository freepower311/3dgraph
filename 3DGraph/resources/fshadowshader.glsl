#version 130

out vec4 fragColor;
//out float fragColor;

void main()
{
    fragColor = vec4(gl_FragCoord.z);
    //fragColor = gl_FragCoord.z;
}
