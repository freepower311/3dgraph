#version 330 core

varying lowp vec3 col;

void main()
{
   gl_FragColor.xyz = col;
   gl_FragColor.w = 1.0;
}
