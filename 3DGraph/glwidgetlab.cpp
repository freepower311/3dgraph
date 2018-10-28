#include "glwidgetlab.h"

GLWidgetLab::GLWidgetLab(QWidget* parent):GLEngineWidget(parent)
{
    m_vshaderPath = ":/vshader.glsl";
    m_fshaderPath = ":/fshader.glsl";
}

void GLWidgetLab::initializeGL()
{
    initializeOpenGLFunctions();
    m_lightPosition = {0.0,0.0,0.0,1.0};

    m_objLoader.load(":/cube.obj");
    loadShaders();
    initTextures();

    //todo: убрать в engine
    GLuint vertexBuf;
    GLuint texCoordBuf;
    GLuint normalsBuf;
    glGenBuffers(1, &vertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glBufferData(GL_ARRAY_BUFFER, m_objLoader.getVertices()->count() * sizeof(float), m_objLoader.getVertices()->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_positionAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers( 1, &texCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, m_objLoader.getUvs()->count() * sizeof(float), m_objLoader.getUvs()->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_texCoordAttr, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers( 1, &normalsBuf);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuf);
    glBufferData(GL_ARRAY_BUFFER, m_objLoader.getNormals()->count() * sizeof(float), m_objLoader.getNormals()->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_normalsAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
}

void GLWidgetLab::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2,0.2,0.2,1.0);

    m_qShaderProgram->bind();
    glEnableVertexAttribArray(m_positionAttr);
    glEnableVertexAttribArray(m_texCoordAttr);
    glEnableVertexAttribArray(m_normalsAttr);

    m_texture->bind();
    QMatrix4x4 viewMatrix;
    m_eye = {m_cameraX, m_cameraY, m_cameraZ};
    m_center = {m_cameraX - (float)sin(m_cameraAngleX / 180.0f * PI),
                m_cameraY + (float)tan(m_cameraAngleY / 180.0f * PI),
                m_cameraZ - (float)cos(m_cameraAngleX / 180.0f * PI)};
    viewMatrix.lookAt(m_eye,m_center,m_up);
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(0.0,0.0,-6.0);
    modelMatrix.rotate(30.0,1.0,1.0,0.0);


    glUniformMatrix4fv(m_matrixAttr, 1 , 0, (m_projection*viewMatrix*modelMatrix).data());
    glUniformMatrix4fv(m_viewMatrixAttr, 1 , 0, (viewMatrix*modelMatrix).data());
    glUniformMatrix4fv(m_normalMatrixAttr, 1 , 0, (viewMatrix*modelMatrix).transposed().inverted().data());
    //move light for debugging
    m_lightPosition[0] += 0.02;
    if (m_lightPosition[0] > 3.0)
        m_lightPosition[0] = -3.0;
    QVector4D viewSpaceLightPosition  = viewMatrix*m_lightPosition;
    glUniform3f(m_viewSpaceLightPosition, viewSpaceLightPosition.x(), viewSpaceLightPosition.y(), viewSpaceLightPosition.z());

    glDrawArrays(GL_TRIANGLES, 0, m_objLoader.verticesCount());

    glDisableVertexAttribArray(m_normalsAttr);
    glDisableVertexAttribArray(m_texCoordAttr);
    glDisableVertexAttribArray(m_positionAttr);
    m_qShaderProgram->release();
}
