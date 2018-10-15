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
    const GLfloat zPos = -5.0;
    m_vertexArray = {
        -0.5f, -0.5f, zPos,
        -0.5f, -0.5f, zPos+1.0,
        0.5f, -0.5f, zPos+1.0,
        0.5f, -0.5f, zPos,

        -0.5f, -0.5f, zPos,
        -0.5f, 0.5f, zPos,
        0.5f, 0.5f, zPos,
        0.5f, -0.5f, zPos,

        -0.5f, -0.5f, zPos,
        -0.5f, -0.5f, zPos+1.0,
        -0.5f, 0.5f, zPos+1.0,
        -0.5f, 0.5f, zPos,

        -0.5f, -0.5f, zPos,
        -0.5f, 0.5f, zPos,
        0.5f, 0.5f, zPos+1.0,
        0.5f, -0.5f, zPos+1.0,

        -0.5f, -0.5f, zPos+1.0,
        -0.5f, 0.5f, zPos+1.0,
        0.5f, 0.5f, zPos,
        0.5f, -0.5f, zPos
    };
    m_textureCoordinates = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        0.5f, 1.0f,
        0.5f, 0.0f,

        0.0f, 0.0f,
        0.0f, 1.0f,
        0.5f, 1.0f,
        0.5f, 0.0f,

        0.0f, 0.0f,
        0.0f, 1.0f,
        0.5f, 1.0f,
        0.5f, 0.0f,

        0.5f, 0.0f,
        0.5f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        0.5f, 0.0f,
        0.5f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    loadShaders();
    initTextures();

    GLuint vertexBuf;
    GLuint colorBuf;
    GLuint texCoordBuf;
    glGenBuffers(1, &vertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glBufferData(GL_ARRAY_BUFFER, m_vertexArray.count() * sizeof(GLfloat), m_vertexArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_positionAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers( 1, &colorBuf);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuf);
    glBufferData(GL_ARRAY_BUFFER, m_colorArray.count() * sizeof(GLfloat), m_colorArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers( 1, &texCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, m_textureCoordinates.count() * sizeof(GLfloat), m_textureCoordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_texCoordAttr, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    GLint tex = glGetUniformLocation(m_shaderProgram, "texture");
    glUniform1f(tex, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLWidgetLab2::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2,0.2,0.2,1.0);

    glUseProgram(m_shaderProgram);
    glEnableVertexAttribArray(m_positionAttr);
    glEnableVertexAttribArray(m_colorAttr);
    glEnableVertexAttribArray(m_texCoordAttr);

    texture->bind();
    QMatrix4x4 matrix = projection;
    eye = {cameraX,cameraY,cameraZ};
    center = {cameraX-sin(cameraAngleX/180*PI),cameraY+(tan(cameraAngleY/180*PI)),cameraZ-cos(cameraAngleX/180*PI)};
    matrix.lookAt(eye,center,up);
    glUniformMatrix4fv(m_matrixAttr, 1 , 0, matrix.data());

    glDrawArrays(GL_QUADS, 0, 20);

    glDisableVertexAttribArray(m_texCoordAttr);
    glDisableVertexAttribArray(m_positionAttr);
    glDisableVertexAttribArray(m_colorAttr);
    glUseProgram(0);
}
