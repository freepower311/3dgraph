attribute highp vec4 posAtr;
uniform highp int time;

void main()
{
    gl_Position = vec4(posAtr.x + cos(posAtr.y*10.0 + time*0.05)*0.1,
                       posAtr.y + sin(posAtr.x*5.0 + time*0.05)*0.1,
                       posAtr.z,
                       1);
}
