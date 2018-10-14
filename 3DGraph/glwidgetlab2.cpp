#include "glwidgetlab2.h"

GLWidgetLab2::GLWidgetLab2(QWidget* parent):GLEngineWidget(parent),
    m_circlePointsCount(50)
{
    m_vshaderPath = ":/vshader.glsl";
    m_fshaderPath = ":/fshader.glsl";
}

void GLWidgetLab2::initializeGL()
{
    initializeOpenGLFunctions();
    //Генерация вершин и их цвета
    const GLfloat z = 0.0;
    m_vertexArray = {
        -0.9f, -0.9f, z,
        -0.5f, 0.0f, z,
        -0.1f, -0.9f, z,
        0.1f, 0.1f, z,
        0.1f, 0.4f, z,
        0.5f, 0.4f, z,
        0.5f, 0.1f, z
    };
    m_colorArray = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f
    };
    for(int i = 0; i < m_circlePointsCount; i++)
    {
        m_vertexArray.append(-0.5 + 0.2*cos(i*2*M_PI/m_circlePointsCount));
        m_vertexArray.append(0.5 + 0.2*sin(i*2*M_PI/m_circlePointsCount));
        m_vertexArray.append(z);
    }
    const GLfloat redColor = 1.0;
    const GLfloat greenColor = 1.0;
    const GLfloat blueColor = 0.0;
    for(int i = 0; i < m_circlePointsCount; i++)
    {
        m_colorArray.append(redColor);
        m_colorArray.append(greenColor);
        m_colorArray.append(blueColor);
    }

    loadShaders();

    GLuint vertexBuf;
    GLuint colorBuf;
    glGenBuffers(1, &vertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glBufferData(GL_ARRAY_BUFFER, m_vertexArray.count() * sizeof(GLfloat), m_vertexArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_positionAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers( 1, &colorBuf);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuf);
    glBufferData(GL_ARRAY_BUFFER, m_colorArray.count() * sizeof(GLfloat), m_colorArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void GLWidgetLab2::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2,0.2,0.2,1.0);

    glUseProgram(m_shaderProgram);
    glEnableVertexAttribArray(m_positionAttr);
    glEnableVertexAttribArray(m_colorAttr);

    QMatrix4x4 matrix = projection*translation*rotation;
    glUniformMatrix4fv(m_matrixAttr, 1 , 0, matrix.data());


    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_QUADS, 3, 4);
    glDrawArrays(GL_POLYGON, 7, m_circlePointsCount);


    glDisableVertexAttribArray(m_positionAttr);
    glDisableVertexAttribArray(m_colorAttr);
    glUseProgram(0);
}
