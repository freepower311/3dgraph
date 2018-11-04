#include "glwidgetlab.h"
#include <random>

GLWidgetLab::GLWidgetLab(QWidget* parent):GLEngineWidget(parent)
{
    calcVertices();
    m_vshaderPath = ":/vshader.glsl";
    m_fshaderPath = ":/fshader.glsl";
    m_vShadowShaderPath = ":/vshadowshader.glsl";
    m_fShadowShaderPath = ":/fshadowshader.glsl";
    //m_vertexStorage.loadObj(":/simple_scene.obj");
    m_lightPosition = {0.0,5.0,10.0,1.0};
    m_decelerationCoefficient = 0.9;
    m_cameraSpeedCoefficient = 0.1;
    m_zNear = 0.2;
    m_zFar = 50.0;
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
    shadowMapResolution = 1024;
}

void GLWidgetLab::paintGL()
{
    Q_ASSERT(m_qShaderProgram != nullptr);
    Q_ASSERT(m_texture != nullptr);
    Q_ASSERT(m_cubeTexture != nullptr);
    Q_ASSERT(m_qShadowShaderProgram != nullptr);


    QMatrix4x4 modelMatrix;
    modelMatrix.translate(0.0,0.0,-6.0);
    //modelMatrix.rotate(-90.0,0.0,1.0,0.0);
    QVector4D viewSpaceLightPosition  = m_viewMatrix * m_lightPosition;

    QMatrix4x4 lightProjectionMatrix;
    lightProjectionMatrix.perspective(45.0f,1.0,5.0f, 100.0f);//(m_fov, aspect, m_zNear, m_zFar)
    QMatrix4x4 lightViewMatrix;
    lightViewMatrix.lookAt(m_lightPosition.toVector3D(), m_lightPosition.toVector3D() + QVector3D(0.106518, -0.963609, -13.6411), QVector3D(0,1,0));
    //qDebug() << m_cameraX << m_cameraY << m_cameraZ;
    QMatrix4x4 lightMatrix = lightProjectionMatrix * lightViewMatrix*modelMatrix * m_viewMatrix.inverted();
    QMatrix4x4 lightMatrix1 = lightProjectionMatrix * lightViewMatrix*modelMatrix;
    m_qShadowShaderProgram->bind();
    m_qShadowShaderProgram->setUniformValue("depthMVP", lightMatrix1);
    m_qShadowShaderProgram->bindAttributeLocation("positionAttr", m_qShaderProgram->attributeLocation(m_positionAttr));

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glViewport(0, 0, shadowMapResolution, shadowMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glClearColor(1.0,1.0,1.0,1.0);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexStorage.coordCount()/3);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_qShadowShaderProgram->release();


    //return;

    m_qShaderProgram->bind();
    m_texture->bind(0);
    m_cubeTexture->bind(1);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.4,0.4,0.4,1.0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);

    m_qShaderProgram->setUniformValue("depthMVP", lightMatrix);
    m_qShaderProgram->setUniformValue("texture", 0);
    m_qShaderProgram->setUniformValue("environmentMap", 1);
    m_qShaderProgram->setUniformValue("shadowMapTex", 2);
    m_qShaderProgram->setUniformValue("mvpMatrix", m_projection*m_viewMatrix*modelMatrix);
    m_qShaderProgram->setUniformValue("modelViewMatrix", m_viewMatrix*modelMatrix);
    m_qShaderProgram->setUniformValue("normalMatrix", (m_viewMatrix*modelMatrix).transposed().inverted());
    m_qShaderProgram->setUniformValue("inverseViewNormalMatrix", m_viewMatrix.transposed());
    m_qShaderProgram->setUniformValue("viewSpaceLightPosition", viewSpaceLightPosition);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexStorage.coordCount()/3);//GL_TRIANGLES

    m_qShaderProgram->release();
}

void GLWidgetLab::processCoordinates()
{
    GLEngineWidget::processCoordinates();
    //move light for debugging
    /*static float moveStep = 0.02;
    m_lightPosition[0] += moveStep;
    if (m_lightPosition[0] > 3.0 || m_lightPosition[0] < -3.0)
        moveStep *= -1;*/
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
        /*0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25,
        0.25, 0.25*/

        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0
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
    int spheresCount = 25;//250
    QVector<QVector3D> clasterCenters = {{-20.0, 0.0, 0.0}, {20.0, 0.0, 0.0}};
    float dispersionXZ = 20.0;
    float dispersionY = 10.0;
    float scale = 0.2;

    foreach(QVector3D clasterCenter, clasterCenters){
        for(int i = 0; i < spheresCount - 1; i++)
        {
            QMatrix4x4 translateMatrix;
            translateMatrix.translate(clasterCenter.x() + (rand()*dispersionXZ/RAND_MAX - dispersionXZ/2),
                                      clasterCenter.y() + (rand()*dispersionY/RAND_MAX - dispersionY/2),
                                      clasterCenter.z() + (rand()*dispersionXZ/RAND_MAX - dispersionXZ/2));
            float currColor = rand()*1.0/RAND_MAX;
            foreach(const QVector4D coord, sphereVertices)
            {
                QVector4D realCoord = scale * coord;
                realCoord = translateMatrix * realCoord;
                vertices->append(realCoord.x());
                vertices->append(realCoord.y());
                vertices->append(realCoord.z());
                normals->append(coord.x()); //normals = coords when center = (0.0,0.0,0.0)
                normals->append(coord.y());
                normals->append(coord.z());
                uvs->append(currColor);
                uvs->append(0.75);
            }
        }
    }
    //add cones
    //todo

    qDebug() << vertices->count()/3 << "triangles";
}

void GLWidgetLab::calcSphere()
{

    const int arcPointCount = 15;//20
    const int arcsCount = 30;//40
    QVector<QVector4D> currArc;
    for(int i = 0; i < arcPointCount; i++)
    {
        QVector4D coord = {(float)cos(i*M_PI/(arcPointCount-1.0)),
                           (float)sin(i*M_PI/(arcPointCount-1.0)),
                           0.0,
                           1.0};
        currArc.append(coord);
    }
    QMatrix4x4 rotateMatrix;
    rotateMatrix.rotate(360.0/arcsCount,1.0,0.0,0.0);
    QVector<QVector4D> nextArc;
    foreach (const QVector4D coord, currArc) {
        nextArc.append(rotateMatrix*coord);
    }
    for(int j = 0; j < arcsCount; j++)
    {
        for(int i = 0; i < arcPointCount - 1; i++)
        {
            sphereVertices.append(currArc[i]);
            sphereVertices.append(currArc[i+1]);
            sphereVertices.append(nextArc[i]);
            sphereVertices.append(currArc[i+1]);
            sphereVertices.append(nextArc[i+1]);
            sphereVertices.append(nextArc[i]);

        }
        currArc = nextArc;
        nextArc.clear();
        foreach (const QVector4D coord, currArc) {
            nextArc.append(rotateMatrix*coord);
        }
    }
}
