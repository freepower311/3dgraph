#include "glenginewidget.h"
#include <QApplication>


GLEngineWidget::GLEngineWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    resetCamera();
    m_drawSceneTimer.setInterval(16);  //62.5 Hz
    m_drawSceneTimer.start();
    m_processCoordinatesTimer.setInterval(8);  //125 Hz
    m_processCoordinatesTimer.start();
    connect(&m_drawSceneTimer, SIGNAL(timeout()), this, SLOT(update()));
    connect(&m_processCoordinatesTimer, SIGNAL(timeout()), this, SLOT(processCoordinates()));
    //default constants
    m_lightPosition = {0.0, 1.0, 0.0, 1.0};
    m_decelerationCoefficient = 0.9;
    m_cameraSpeedCoefficient = 0.1;
    m_zNear = 0.2;
    m_zFar = 50.0;
    m_fov = 45.0;
    m_rotationSens = 0.2;
    m_shadowMapResolution = 1024;
    m_viewSpaceLightDir = {0, 0, -1};
    m_spotOuterAngle = cos(20.0 * M_PI / 180.0);
    m_spotInnerAngle = cos(20.0 * M_PI / 180.0);
}

void GLEngineWidget::resetCamera(){
    m_currentCameraSpeedCoefficient = m_cameraSpeedCoefficient;
    m_up = {0, 1, 0};
    m_cameraX = 0;
    m_cameraY = 0;
    m_cameraZ = 0;
    m_cameraXSpeed = 0;
    m_cameraYSpeed = 0;
    m_cameraZSpeed = 0;
    m_cameraAngleX = 0;
    m_cameraAngleY = 0;
}

void GLEngineWidget::loadShaders()
{
    m_qShaderProgram = new QOpenGLShaderProgram(this);
    m_qShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vshaderPath);
    m_qShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fshaderPath);
    m_qShaderProgram->link();

    m_qShadowShaderProgram = new QOpenGLShaderProgram(this);
    m_qShadowShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vShadowShaderPath);
    m_qShadowShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fShadowShaderPath);
    m_qShadowShaderProgram->link();
}

void GLEngineWidget::mousePressEvent(QMouseEvent *e)
{
    m_mousePressPosition = QVector2D(e->localPos());
    QApplication::setOverrideCursor(Qt::BlankCursor);
    QOpenGLWidget::mousePressEvent(e);
}

void GLEngineWidget::mouseMoveEvent(QMouseEvent *e)
{
    QVector2D diff = QVector2D(e->localPos()) - m_mousePressPosition;
    m_mousePressPosition = QVector2D(e->localPos());
    m_cameraAngleX += diff.x() * m_rotationSens;
    m_cameraAngleY += diff.y() * m_rotationSens;
    if (m_cameraAngleY >= 90.0)
        m_cameraAngleY = 89.9;
    if (m_cameraAngleY <= -90.0)
        m_cameraAngleY = -89.9;
    QOpenGLWidget::mouseMoveEvent(e);
}

void GLEngineWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    QOpenGLWidget::mouseReleaseEvent(e);
}

void GLEngineWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_F1:
        resetCamera();
        break;
    case 1060: //ф
    case Qt::Key_A:
        m_cameraXSpeed = (float)sin(( m_cameraAngleX - 90) / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        m_cameraZSpeed = (float)cos(( m_cameraAngleX - 90) / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        break;
    case 1062: //ц
    case Qt::Key_W:
        m_cameraXSpeed = -(float)sin(m_cameraAngleX / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        m_cameraZSpeed = -(float)cos(m_cameraAngleX / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        break;
    case 1067: //ы
    case Qt::Key_S:
        m_cameraXSpeed = (float)sin(m_cameraAngleX / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        m_cameraZSpeed = (float)cos(m_cameraAngleX / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        break;
    case 1042: //в
    case Qt::Key_D:
        m_cameraXSpeed = (float)sin(( m_cameraAngleX + 90) / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        m_cameraZSpeed = (float)cos(( m_cameraAngleX + 90) / 180 * M_PI) * m_currentCameraSpeedCoefficient;
        break;
    case 1071: //я
    case Qt::Key_Z:
        m_cameraYSpeed = -m_currentCameraSpeedCoefficient;
        break;
    case 1063: //ч
    case Qt::Key_X:
        m_cameraYSpeed = m_currentCameraSpeedCoefficient;
        break;
    case 1057: //с
    case Qt::Key_C:
        m_currentCameraSpeedCoefficient *= 0.5;
        break;
    case 1052: //м
    case Qt::Key_V:
        m_currentCameraSpeedCoefficient *= 2;
        break;
    default:
        break;
    }
    QOpenGLWidget::keyPressEvent(e);
}

void GLEngineWidget::resizeGL(int w, int h)
{
    m_screenWidth = w;
    m_screenHeight = h;
    const qreal aspect = qreal(m_screenWidth) / qreal(m_screenHeight ? m_screenHeight : 1);
    m_projection.setToIdentity();
    m_projection.perspective(m_fov, aspect, m_zNear, m_zFar);
}

void GLEngineWidget::initTextures()
{
    if (m_texturePath != "")
    {
        m_texture = new QOpenGLTexture(QImage(m_texturePath).mirrored());
        m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
        m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
        m_texture->setWrapMode(QOpenGLTexture::Repeat);
        m_texture->setMaximumAnisotropy(16.0f);
    }
    if (m_cubeTexturesPath.count() == 6)
    {
        m_cubeTexture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
        const QImage posx = QImage(m_cubeTexturesPath[0]).convertToFormat(QImage::Format_RGBA8888);
        const QImage posy = QImage(m_cubeTexturesPath[1]).convertToFormat(QImage::Format_RGBA8888);
        const QImage posz = QImage(m_cubeTexturesPath[2]).convertToFormat(QImage::Format_RGBA8888);
        const QImage negx = QImage(m_cubeTexturesPath[3]).convertToFormat(QImage::Format_RGBA8888);
        const QImage negy = QImage(m_cubeTexturesPath[4]).convertToFormat(QImage::Format_RGBA8888);
        const QImage negz = QImage(m_cubeTexturesPath[5]).convertToFormat(QImage::Format_RGBA8888);
        m_cubeTexture->create();
        m_cubeTexture->setSize(posx.width(), posx.height(), posx.depth());
        m_cubeTexture->setFormat(QOpenGLTexture::RGBA8_UNorm);
        m_cubeTexture->allocateStorage();
        m_cubeTexture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,
                                QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                                (const void*)posx.constBits(), 0);
        m_cubeTexture->setData(0, 0, QOpenGLTexture::CubeMapPositiveY,
                                QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                                (const void*)posy.constBits(), 0);
        m_cubeTexture->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ,
                                QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                                (const void*)posz.constBits(), 0);
        m_cubeTexture->setData(0, 0, QOpenGLTexture::CubeMapNegativeX,
                                QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                                (const void*)negx.constBits(), 0);
        m_cubeTexture->setData(0, 0, QOpenGLTexture::CubeMapNegativeY,
                                QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                                (const void*)negy.constBits(), 0);
        m_cubeTexture->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ,
                                QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                                (const void*)negz.constBits(), 0);
        m_cubeTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
        m_cubeTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        m_cubeTexture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
        m_cubeTexture->generateMipMaps();
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->textureId());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTexture->textureId());
}

void GLEngineWidget::processCoordinates()
{
    m_cameraX += m_cameraXSpeed;
    m_cameraZ += m_cameraZSpeed;
    m_cameraY += m_cameraYSpeed;
    m_cameraXSpeed *= m_decelerationCoefficient;
    m_cameraZSpeed *= m_decelerationCoefficient;
    m_cameraYSpeed *= m_decelerationCoefficient;
    m_eye = {m_cameraX, m_cameraY, m_cameraZ};
    m_center = {m_cameraX - (float)sin(m_cameraAngleX / 180.0f * M_PI),
                m_cameraY + (float)tan(m_cameraAngleY / 180.0f * M_PI),
                m_cameraZ - (float)cos(m_cameraAngleX / 180.0f * M_PI)};
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(m_eye,m_center,m_up);
    QMatrix4x4 spotDirectionMatrix;
    QVector3D spotCenter = { -(float)sin(m_cameraAngleX / 180.0f * M_PI),
                              (float)tan(m_cameraAngleY / 180.0f * M_PI),
                             -(float)cos(m_cameraAngleX / 180.0f * M_PI)};
    spotDirectionMatrix.lookAt({0, 0, 0}, spotCenter, {0, 1, 0});
    m_currViewSpaceLightDir = spotDirectionMatrix * m_viewSpaceLightDir;
}

void GLEngineWidget::initializeGL()
{
    initializeOpenGLFunctions();
    loadShaders();
    initTextures();
    Q_ASSERT(m_qShaderProgram != nullptr);
    GLuint vertexBuf;
    GLuint texCoordBuf;
    GLuint normalsBuf;
    glGenBuffers(1, &vertexBuf);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
    glBufferData(GL_ARRAY_BUFFER, m_vertexStorage.getVertices()->count() * sizeof(float), m_vertexStorage.getVertices()->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_qShaderProgram->attributeLocation(m_positionAttr), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &texCoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuf);
    glBufferData(GL_ARRAY_BUFFER, m_vertexStorage.getUvs()->count() * sizeof(float), m_vertexStorage.getUvs()->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_qShaderProgram->attributeLocation(m_texCoordAttr), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &normalsBuf);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuf);
    glBufferData(GL_ARRAY_BUFFER, m_vertexStorage.getNormals()->count() * sizeof(float), m_vertexStorage.getNormals()->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_qShaderProgram->attributeLocation(m_normalsAttr), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenTextures(1, &shadowMapTexture );
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture );
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT32, m_shadowMapResolution, m_shadowMapResolution, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    float zeros[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //GL_REPEAT GL_CLAMP_TO_EDGE GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, zeros);
    glBindTexture(GL_TEXTURE_2D, 0);
    glGenFramebuffers(1, &m_shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qWarning() << "glCheckFramebufferStatus NOT OK";
    glBindFramebuffer(GL_FRAMEBUFFER, 0 );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(m_qShaderProgram->attributeLocation(m_positionAttr));
    glEnableVertexAttribArray(m_qShaderProgram->attributeLocation(m_texCoordAttr));
    glEnableVertexAttribArray(m_qShaderProgram->attributeLocation(m_normalsAttr));
    m_currentCameraSpeedCoefficient = m_cameraSpeedCoefficient;
    QOpenGLWidget::initializeGL();
}

