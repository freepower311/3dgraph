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
}

void GLEngineWidget::resetCamera(){
    m_up = {0,1,0};
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
    //compile shaders
    m_qShaderProgram = new QOpenGLShaderProgram(this);
    m_qShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vshaderPath);
    m_qShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fshaderPath);
    m_qShaderProgram->link();
    m_positionAttr = m_qShaderProgram->attributeLocation("positionAttr");
    m_texCoordAttr = m_qShaderProgram->attributeLocation("texCoordIn");
    m_matrixAttr =  m_qShaderProgram->uniformLocation("mvpMatrix");
    m_normalsAttr = m_qShaderProgram->attributeLocation("normalsIn");
    m_normalMatrixAttr = m_qShaderProgram->uniformLocation("normalMatrix");
    m_viewMatrixAttr = m_qShaderProgram->uniformLocation("modelViewMatrix");
    m_viewSpaceLightPosition = m_qShaderProgram->uniformLocation("viewSpaceLightPosition");
    m_inverseViewNormalMatrixAttr = m_qShaderProgram->uniformLocation("inverseViewNormalMatrix");
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
    static const double rotationSens = 0.2;
    m_cameraAngleX += diff.x()*rotationSens;
    m_cameraAngleY += diff.y()*rotationSens;
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
    //todo: убрать в конфиг?
    static const double cameraSpeedCoefficient = 0.1;
    switch (e->key()) {
    case Qt::Key_F1:
        resetCamera();
        break;
    case 1060: //ф
    case Qt::Key_A:
        m_cameraXSpeed = (float)sin(( m_cameraAngleX - 90)/180*PI) * cameraSpeedCoefficient;
        m_cameraZSpeed = (float)cos(( m_cameraAngleX - 90)/180*PI) * cameraSpeedCoefficient;
        break;
    case 1062: //ц
    case Qt::Key_W:
        m_cameraXSpeed = -(float)sin(m_cameraAngleX/180*PI) * cameraSpeedCoefficient;
        m_cameraZSpeed = -(float)cos(m_cameraAngleX/180*PI) * cameraSpeedCoefficient;
        break;
    case 1067: //ы
    case Qt::Key_S:
        m_cameraXSpeed = (float)sin(m_cameraAngleX/180*PI) * cameraSpeedCoefficient;
        m_cameraZSpeed = (float)cos(m_cameraAngleX/180*PI) * cameraSpeedCoefficient;
        break;
    case 1042: //в
    case Qt::Key_D:
        m_cameraXSpeed = (float)sin(( m_cameraAngleX + 90)/180*PI) * cameraSpeedCoefficient;
        m_cameraZSpeed = (float)cos(( m_cameraAngleX + 90)/180*PI) * cameraSpeedCoefficient;
        break;
    case 1071: //я
    case Qt::Key_Z:
        m_cameraYSpeed = -cameraSpeedCoefficient;
        break;
    case 1063: //ч
    case Qt::Key_X:
        m_cameraYSpeed = cameraSpeedCoefficient;
        break;
    default:
        break;
    }
    QOpenGLWidget::keyPressEvent(e);
}

void GLEngineWidget::resizeGL(int w, int h)
{
    const qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear = 0.2, zFar = 50.0, fov = 45.0;
    m_projection.setToIdentity();
    m_projection.perspective(fov, aspect, zNear, zFar);
}

void GLEngineWidget::initTextures()
{
    //todo: Имена файлов передавать через поля
    m_texture = new QOpenGLTexture(QImage(":/metal.png").mirrored());
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
    m_texture->setMaximumAnisotropy(16.0f);
    m_cubeTexture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
    const QImage posx = QImage(":/cube0.png").convertToFormat(QImage::Format_RGBA8888);
    const QImage posy = QImage(":/cube2.png").convertToFormat(QImage::Format_RGBA8888);
    const QImage posz = QImage(":/cube4.png").convertToFormat(QImage::Format_RGBA8888);
    const QImage negx = QImage(":/cube1.png").convertToFormat(QImage::Format_RGBA8888);
    const QImage negy = QImage(":/cube3.png").convertToFormat(QImage::Format_RGBA8888);
    const QImage negz = QImage(":/cube5.png").convertToFormat(QImage::Format_RGBA8888);
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

void GLEngineWidget::processCoordinates()
{
    static const float decelerationCoefficient = 0.9;
    m_cameraX += m_cameraXSpeed;
    m_cameraZ += m_cameraZSpeed;
    m_cameraY += m_cameraYSpeed;
    m_cameraXSpeed *= decelerationCoefficient;
    m_cameraZSpeed *= decelerationCoefficient;
    m_cameraYSpeed *= decelerationCoefficient;
}
