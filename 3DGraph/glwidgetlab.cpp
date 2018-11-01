#include "glwidgetlab.h"

GLWidgetLab::GLWidgetLab(QWidget* parent):GLEngineWidget(parent)
{
    m_vshaderPath = ":/vshader.glsl";
    m_fshaderPath = ":/fshader.glsl";
    m_vertexStorage.loadObj(":/simple_scene.obj");
    m_lightPosition = {0.0,1.0,0.0,1.0};
    m_decelerationCoefficient = 0.9;
    m_cameraSpeedCoefficient = 0.1;
    m_zNear = 0.2;
    m_zFar = 50.0;
    m_fov = 45.0;
    m_texturePath = ":/metal.png";
    m_cubeTexturesPath = {":/cube0.png",
                          ":/cube2.png",
                          ":/cube4.png",
                          ":/cube1.png",
                          ":/cube3.png",
                          ":/cube5.png"};
    m_positionAttr = "positionAttr";
    m_texCoordAttr = "texCoordIn";
    m_normalsAttr = "normalsIn";
    m_rotationSens = 0.2;
}

void GLWidgetLab::paintGL()
{
    Q_ASSERT(m_qShaderProgram != nullptr);
    Q_ASSERT(m_texture != nullptr);
    Q_ASSERT(m_cubeTexture != nullptr);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4,0.4,0.4,1.0);

    m_qShaderProgram->bind();
    m_texture->bind(0);
    m_cubeTexture->bind(1);


    QMatrix4x4 modelMatrix;
    modelMatrix.translate(0.0,-1.0,-6.0);
    modelMatrix.rotate(-90.0,0.0,1.0,0.0);
    QVector4D viewSpaceLightPosition  = m_viewMatrix * m_lightPosition;

    m_qShaderProgram->setUniformValue("texture", 0);
    m_qShaderProgram->setUniformValue("environmentMap", 1);
    m_qShaderProgram->setUniformValue("mvpMatrix", m_projection*m_viewMatrix*modelMatrix);
    m_qShaderProgram->setUniformValue("modelViewMatrix", m_viewMatrix*modelMatrix);
    m_qShaderProgram->setUniformValue("normalMatrix", (m_viewMatrix*modelMatrix).transposed().inverted());
    m_qShaderProgram->setUniformValue("inverseViewNormalMatrix", m_viewMatrix.transposed());
    m_qShaderProgram->setUniformValue("viewSpaceLightPosition", viewSpaceLightPosition);

    glDrawArrays(GL_TRIANGLES, 0, m_vertexStorage.verticesCount());

    m_qShaderProgram->release();
}

void GLWidgetLab::processCoordinates()
{
    GLEngineWidget::processCoordinates();
    //move light for debugging
    static float moveStep = 0.02;
    m_lightPosition[0] += moveStep;
    if (m_lightPosition[0] > 3.0 || m_lightPosition[0] < -3.0)
        moveStep *= -1;
}
