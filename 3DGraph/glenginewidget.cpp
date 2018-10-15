#include "glenginewidget.h"
#include <QFile>
#include <QApplication>

GLEngineWidget::GLEngineWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    resetCamera();
}
void GLEngineWidget::resetCamera(){
    up = {0,1,0};
    cameraX = 0;
    cameraY = 0;
    cameraZ = 0;
    cameraAngleX = 0;
    cameraAngleY = 0;
}

std::string readFromFile(QString &path)
{
    QFile file(path);
    QString textFromFile;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(file.isOpen())
    {
        while(!file.atEnd())
        {
            textFromFile += file.readLine().data();
        }
    }
    return textFromFile.toStdString();
}

void GLEngineWidget::loadShaders()
{
    //создаем шейдеры
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    //читаем текст шейдеров из файлов
    std::string vertexShaderCode = readFromFile(m_vshaderPath);
    std::string fragmentShaderCode = readFromFile(m_fshaderPath);

    GLint result = GL_FALSE;
    int infoLogLength;

    //Компилируем вершинный шейдер
    qDebug() << "Compiling shader : " <<  m_vshaderPath;
    char const * vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer , NULL);
    glCompileShader(vertexShaderID);

    // Устанавливаем параметры
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> vertexShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
    if(&vertexShaderErrorMessage[0])
        qDebug() << &vertexShaderErrorMessage[0];

    //Компилируем фрагментный шейдер
    qDebug() << "Compiling shader : " <<  m_fshaderPath;
    char const * fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer , NULL);
    glCompileShader(fragmentShaderID);

    //Устанавливаем параметры
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> fragmentShaderErrorMessage(infoLogLength);
    glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
    if(&fragmentShaderErrorMessage[0])
        qDebug() << &fragmentShaderErrorMessage[0];

    qDebug() << "Linking program\n";
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShaderID);
    glAttachShader(m_shaderProgram, fragmentShaderID);
    glLinkProgram(m_shaderProgram);

    //Устанавливаем параметры
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &result);
    glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programErrorMessage( std::max(infoLogLength, int(1)));
    glGetProgramInfoLog(m_shaderProgram, infoLogLength, NULL, &programErrorMessage[0]);
    if (&programErrorMessage[0])
        qDebug() << &programErrorMessage[0];

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    m_positionAttr = glGetAttribLocation(m_shaderProgram, "positionAttr");
    m_colorAttr = glGetAttribLocation(m_shaderProgram, "colorAttr");
    m_texCoordAttr = glGetAttribLocation(m_shaderProgram, "texCoordIn");
    m_matrixAttr = glGetUniformLocation(m_shaderProgram, "mvp_matrix");
}

void GLEngineWidget::mousePressEvent(QMouseEvent *e)
{
    mousePressPosition = QVector2D(e->localPos());
    QApplication::setOverrideCursor(Qt::BlankCursor);
}

void GLEngineWidget::mouseMoveEvent(QMouseEvent *e)
{
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
    mousePressPosition = QVector2D(e->localPos());
    const double rotationSens = 0.2;
    cameraAngleX += diff.x()*rotationSens;
    cameraAngleY += diff.y()*rotationSens;
    update();
}

void GLEngineWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void GLEngineWidget::keyPressEvent(QKeyEvent *e)
{
    const double cameraSpeed = 0.1;
    switch (e->key()) {
    case Qt::Key_F1:
        resetCamera();
        break;
    case Qt::Key_A:
        cameraX += (float)sin(( cameraAngleX - 90)/180*PI) * cameraSpeed;
        cameraZ += (float)cos(( cameraAngleX - 90)/180*PI) * cameraSpeed;
        break;
    case Qt::Key_W:
        cameraX -= (float)sin(cameraAngleX/180*PI) * cameraSpeed;
        cameraZ -= (float)cos(cameraAngleX/180*PI) * cameraSpeed;
        break;
    case Qt::Key_S:
        cameraX += (float)sin(cameraAngleX/180*PI) * cameraSpeed;
        cameraZ += (float)cos(cameraAngleX/180*PI) * cameraSpeed;
        break;
    case Qt::Key_D:
        cameraX += (float)sin(( cameraAngleX + 90)/180*PI) * cameraSpeed;
        cameraZ += (float)cos(( cameraAngleX + 90)/180*PI) * cameraSpeed;
        break;
    default:
        break;
    }
    update();

    QOpenGLWidget::keyPressEvent(e);
}

void GLEngineWidget::resizeGL(int w, int h)
{
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear = 0.0, zFar = 7.0, fov = 45.0;
    projection.setToIdentity();
    projection.perspective(fov, aspect, zNear, zFar);
}

void GLEngineWidget::initTextures()
{
    // Load cube.png image
    texture = new QOpenGLTexture(QImage(":/wood.png").mirrored());
    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}
