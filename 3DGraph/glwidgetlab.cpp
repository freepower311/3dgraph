#include "glwidgetlab.h"

GLWidgetLab::GLWidgetLab(QWidget* parent):GLEngineWidget(parent)
{
    m_vshaderPath = ":/vshader.glsl";
    m_fshaderPath = ":/fshader.glsl";
}

void GLWidgetLab::initializeGL()
{
    initializeOpenGLFunctions();
    //Генерация вершин и их координат на текстуре
    const GLfloat zPos = -5.0;
    m_vertexArray = {
        -0.5f, -0.5f, zPos,
        -0.5f, -0.5f, zPos+1.0f,
        0.5f, -0.5f, zPos+1.0f,
        0.5f, -0.5f, zPos,

        -0.5f, -0.5f, zPos,
        -0.5f, 0.5f, zPos,
        0.5f, 0.5f, zPos,
        0.5f, -0.5f, zPos,

        -0.5f, -0.5f, zPos,
        -0.5f, -0.5f, zPos+1.0f,
        -0.5f, 0.5f, zPos+1.0f,
        -0.5f, 0.5f, zPos,

        -0.5f, -0.5f, zPos,
        -0.5f, 0.5f, zPos,
        0.0f, 0.5f, zPos+0.5f,
        0.0f, -0.5f, zPos+0.5f,

        -0.5f, -0.5f, zPos+1.0f,
        -0.5f, 0.5f, zPos+1.0f,
        0.5f, 0.5f, zPos,
        0.5f, -0.5f, zPos,

        0.0f, -0.5f, zPos+0.5f,
        0.0f, 0.5f, zPos+0.5f,
        0.5f, 0.5f, zPos+1.0f,
        0.5f, -0.5f, zPos+1.0f
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
        0.75f, 1.0f,
        0.75f, 0.0f,

        0.5f, 0.0f,
        0.5f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        0.75f, 0.0f,
        0.75f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    loadShaders();
    initTextures();

    GLuint vertexBuf;
    GLuint texCoordBuf;
    glGenBuffers(1, &vertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glBufferData(GL_ARRAY_BUFFER, m_vertexArray.count() * sizeof(GLfloat), m_vertexArray.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_positionAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers( 1, &texCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, m_textureCoordinates.count() * sizeof(GLfloat), m_textureCoordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_texCoordAttr, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
}

void GLWidgetLab::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2,0.2,0.2,1.0);

    glUseProgram(m_shaderProgram);
    glEnableVertexAttribArray(m_positionAttr);
    glEnableVertexAttribArray(m_texCoordAttr);

    m_texture->bind();
    QMatrix4x4 matrix = m_projection;
    m_eye = {m_cameraX, m_cameraY, m_cameraZ};
    m_center = {m_cameraX-(float)sin(m_cameraAngleX/180.0f*PI),
              m_cameraY+((float)tan(m_cameraAngleY/180.0f*PI)),
              m_cameraZ-(float)cos(m_cameraAngleX/180.0f*PI)};
    matrix.lookAt(m_eye,m_center,m_up);
    glUniformMatrix4fv(m_matrixAttr, 1 , 0, matrix.data());

    glDrawArrays(GL_QUADS, 0, 24);

    glDisableVertexAttribArray(m_texCoordAttr);
    glDisableVertexAttribArray(m_positionAttr);
    glUseProgram(0);
}
