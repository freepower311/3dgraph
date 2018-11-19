#include "glwidgetlab.h"
#include <random>

GLWidgetLab::GLWidgetLab(QWidget* parent):GLEngineWidget(parent)
{

    m_vshaderPath = ":/vshader.glsl";
    m_fshaderPath = ":/fshader.glsl";
    m_vShadowShaderPath = ":/vshadowshader.glsl";
    m_fShadowShaderPath = ":/fshadowshader.glsl";
    m_lightPosition = {0.0,10.0,7.0,1.0};
    m_decelerationCoefficient = 0.9;
    m_cameraSpeedCoefficient = 0.1;
    m_zNear = 0.2;
    m_zFar = 100.0;
    m_fov = 45.0;
    m_texturePath = ":/colors.png";
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
    m_shadowMapResolution = 2048;
    m_viewSpaceLightDir = {0, -0.9, -1};
    m_viewSpaceLightDir.normalize();
    const float outerAngle = 30.0;
    const float innerAngle = 26.0;
    m_spotOuterAngle = cos(outerAngle * M_PI / 180.0f);
    m_spotInnerAngle = cos(innerAngle * M_PI / 180.0f);

    m_spheresPerClaster = 25;
    m_spheresArcPointCount = 15;
    m_spheresArcsCount = 30;
    m_spheresClastersCenters = {{-20.0, 0.0, 0.0}, {20.0, 0.0, 0.0}};
    m_spheresDispersionXZ = 20.0;
    m_spheresDispersionY = 10.0;
    m_spheresScale = 0.2;
    calcVertices();
}

void GLWidgetLab::paintGL()
{
    Q_ASSERT(m_qShaderProgram != nullptr);
    Q_ASSERT(m_texture != nullptr);
    Q_ASSERT(m_cubeTexture != nullptr);
    Q_ASSERT(m_qShadowShaderProgram != nullptr);

    QMatrix4x4 modelMatrix;
    modelMatrix.translate(0.0, 0.0, -6.0);
    modelMatrix.rotate(0.0, 0.0, 1.0, 0.0);
    QVector4D viewSpaceLightPosition  = m_viewMatrix * m_lightPosition;
    QMatrix4x4 lightProjectionMatrix;
    lightProjectionMatrix.perspective(70.0f, 1.0, 5.0f, 100.0f); //(fov, aspect, zNear, zFar)
    QMatrix4x4 lightViewMatrix;
    lightViewMatrix.lookAt(m_lightPosition.toVector3D(), m_lightPosition.toVector3D() + m_viewSpaceLightDir, QVector3D(0, 1, 0));
    QMatrix4x4 lightMatrix = lightProjectionMatrix * lightViewMatrix * m_viewMatrix.inverted();
    QMatrix4x4 lightMatrix1 = lightProjectionMatrix * lightViewMatrix * modelMatrix;

    m_qShadowShaderProgram->bind();
    m_qShadowShaderProgram->setUniformValue("depthMVP", lightMatrix1);
    m_qShadowShaderProgram->bindAttributeLocation("positionAttr", m_qShaderProgram->attributeLocation(m_positionAttr));
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5, 10);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glViewport(0, 0, m_shadowMapResolution, m_shadowMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexStorage.coordCount());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);
    m_qShadowShaderProgram->release();

    m_qShaderProgram->bind();
    m_texture->bind(0);
    m_cubeTexture->bind(1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4,0.4,0.4,1.0);
    m_qShaderProgram->setUniformValue("depthMVP", lightMatrix);
    m_qShaderProgram->setUniformValue("texture", 0);
    m_qShaderProgram->setUniformValue("environmentMap", 1);
    m_qShaderProgram->setUniformValue("shadowMapTex", 2);
    m_qShaderProgram->setUniformValue("mvpMatrix", m_projection*m_viewMatrix*modelMatrix);
    m_qShaderProgram->setUniformValue("modelViewMatrix", m_viewMatrix*modelMatrix);
    m_qShaderProgram->setUniformValue("normalMatrix", (m_viewMatrix*modelMatrix).transposed().inverted());
    m_qShaderProgram->setUniformValue("inverseViewNormalMatrix", m_viewMatrix.transposed());
    m_qShaderProgram->setUniformValue("viewSpaceLightPosition", viewSpaceLightPosition);
    m_qShaderProgram->setUniformValue("viewSpaceLightDir", m_currViewSpaceLightDir);
    m_qShaderProgram->setUniformValue("spotOuterAngle", m_spotOuterAngle);
    m_qShaderProgram->setUniformValue("spotInnerAngle", m_spotInnerAngle);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexStorage.coordCount());
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

void GLWidgetLab::calcVertices()
{
    calcSphere();
    QVector<float>* vertices = m_vertexStorage.getVertices();
    QVector<float>* uvs = m_vertexStorage.getUvs();
    QVector<float>* normals = m_vertexStorage.getNormals();
    //add ground
    QVector<float> groundVertices = {
        -15.0, -2.0, -15.0,
        -15.0, -2.0,  15.0,
         15.0, -2.0,  15.0,
         15.0, -2.0,  15.0,
         15.0, -2.0, -15.0,
        -15.0, -2.0, -15.0
    };
    QVector<float> groundUvs = {
        0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25
    };
    QVector<float> groundNormals = {
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0
    };
    vertices->append(groundVertices);
    normals->append(groundNormals);
    uvs->append(groundUvs);
    //add spheres
    foreach(QVector3D clasterCenter, m_spheresClastersCenters){
        for(int i = 0; i < m_spheresPerClaster - 1; i++)
        {
            QMatrix4x4 translateMatrix;
            translateMatrix.translate(clasterCenter.x() + (rand() * m_spheresDispersionXZ / RAND_MAX - m_spheresDispersionXZ / 2),
                                      clasterCenter.y() + (rand() * m_spheresDispersionY  / RAND_MAX - m_spheresDispersionY  / 2),
                                      clasterCenter.z() + (rand() * m_spheresDispersionXZ / RAND_MAX - m_spheresDispersionXZ / 2));
            float currColor = rand() * 1.0 / RAND_MAX;
            foreach(const QVector4D coord, m_sphereVertices)
            {
                QVector4D realCoord = m_spheresScale * coord;
                realCoord = translateMatrix * realCoord;
                vertices->append(realCoord.x());
                vertices->append(realCoord.y());
                vertices->append(realCoord.z());
                normals->append(coord.x()); //normals == coords when center == (0.0,0.0,0.0)
                normals->append(coord.y());
                normals->append(coord.z());
                uvs->append(currColor); //0.01 - green, 0.99 - red
                uvs->append(0.75);
            }
        }
    }
    //add cones
    ModelStorage coneModel;
    coneModel.loadObj(":/cone.obj");
    {
        QMatrix4x4 translateMatrix;
        translateMatrix.translate(0, -2.05, 0);
        for(int i = 0; i < coneModel.getVertices()->count(); i += 3)
        {
            QVector4D realCoord(coneModel.getVertices()->at(i),
                                coneModel.getVertices()->at(i + 1),
                                coneModel.getVertices()->at(i + 2),
                                1.0);
            realCoord = translateMatrix * realCoord;
            vertices->append(realCoord.x());
            vertices->append(realCoord.y());
            vertices->append(realCoord.z());
        }
        normals->append(*coneModel.getNormals());
        uvs->append(*coneModel.getUvs());
    }
    qDebug() << m_vertexStorage.coordCount() << "triangles";
}

void GLWidgetLab::calcSphere()
{
    QVector<QVector4D> currArc;
    for(int i = 0; i < m_spheresArcPointCount; i++)
    {
        currArc.append({(float)cos(i * M_PI / (m_spheresArcPointCount - 1.0)),
                        (float)sin(i * M_PI / (m_spheresArcPointCount - 1.0)),
                        0.0,
                        1.0});
    }
    QMatrix4x4 rotateMatrix;
    rotateMatrix.rotate(360.0 / m_spheresArcsCount, 1.0, 0.0, 0.0);
    QVector<QVector4D> nextArc;
    for(int j = 0; j < m_spheresArcsCount; j++)
    {
        nextArc.clear();
        foreach (const QVector4D coord, currArc) {
            nextArc.append(rotateMatrix * coord);
        }
        for(int i = 0; i < m_spheresArcPointCount - 1; i++)
        {
            m_sphereVertices.append(currArc[i]);
            m_sphereVertices.append(currArc[i + 1]);
            m_sphereVertices.append(nextArc[i]);
            m_sphereVertices.append(currArc[i + 1]);
            m_sphereVertices.append(nextArc[i + 1]);
            m_sphereVertices.append(nextArc[i]);
        }
        currArc = nextArc;
    }
}
