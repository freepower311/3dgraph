#include "glenginewidget.h"
#include <QApplication>


GLEngineWidget::GLEngineWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    resetCamera();
    m_timer.setInterval(16);  //62.5 fps
    m_timer.start();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(processCameraMovements()));
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
    GLint tex = m_qShaderProgram->uniformLocation("texture");
    glUniform1f(tex, 0);
    m_viewSpaceLightPosition = m_qShaderProgram->uniformLocation("viewSpaceLightPosition");
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
    const qreal zNear = 0.2, zFar = 7.0, fov = 45.0;
    m_projection.setToIdentity();
    m_projection.perspective(fov, aspect, zNear, zFar);
}

void GLEngineWidget::initTextures()
{
    m_texture = new QOpenGLTexture(QImage(":/texture.png").mirrored());
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
    m_texture->setMaximumAnisotropy(16.0f);
}

void GLEngineWidget::processCameraMovements()
{
    static const float decelerationCoefficient = 0.9;
    m_cameraX += m_cameraXSpeed;
    m_cameraZ += m_cameraZSpeed;
    m_cameraY += m_cameraYSpeed;
    m_cameraXSpeed *= decelerationCoefficient;
    m_cameraZSpeed *= decelerationCoefficient;
    m_cameraYSpeed *= decelerationCoefficient;
    update();
}
