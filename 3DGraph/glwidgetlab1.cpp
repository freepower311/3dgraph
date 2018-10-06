#include "glwidgetlab1.h"

GLWidgetLab1::GLWidgetLab1(QWidget* parent):GLWidget(parent),
    m_circlePointsCount(50)
{
    m_vshaderPath = ":/vshader.glsl";
    m_fshaderPath = ":/fshader.glsl";
}

void GLWidgetLab1::initializeGL()
{
    initializeOpenGLFunctions();
    m_vertexArray = {
        -0.9f, -0.9f, 0.0f,
        -0.5f, 0.0f, 0.0f,
        -0.1f, -0.9f, 0.0f,
        0.1f, 0.1f, 0.0f,
        0.1f, 0.4f, 0.0f,
        0.5f, 0.4f, 0.0f,
        0.5f, 0.1f, 0.0f
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
        m_vertexArray.append(0.0);
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

    //standard way
    loadShaders();

    //QOpenGLShaderProgram way
    //initShaders();

    glVertexAttribPointer(m_positionAttr, 3, GL_FLOAT, false, 0, m_vertexArray.data());
    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, false, 0, m_colorArray.data());

}

void GLWidgetLab1::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2,0.2,0.2,1.0);

    //standard way
    glUseProgram(m_shaderProgram);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_QUADS, 3, 4);
    glDrawArrays(GL_POLYGON, 7, m_circlePointsCount);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glUseProgram(0);


    //QOpenGLShaderProgram way
    /*
    m_qShaderProgram->bind();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    m_qShaderProgram->release();
    */
}
