#include "glenginewidget.h"
#include <QFile>

GLEngineWidget::GLEngineWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    rotation.setToIdentity();
    translation.setToIdentity();
    translation.translate(0.0, 0.0, -5.0);
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
    m_matrixAttr = glGetUniformLocation(m_shaderProgram, "mvp_matrix");
}

void GLEngineWidget::mousePressEvent(QMouseEvent *e)
{
    mousePressPosition = QVector2D(e->localPos());
}

void GLEngineWidget::mouseMoveEvent(QMouseEvent *e)
{
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
    mousePressPosition = QVector2D(e->localPos());
    QVector3D norma = QVector3D(diff.y(), diff.x(), 0.0).normalized();
    qreal angle = diff.length() / 3.0;
    rotationAxis = (rotationAxis + norma).normalized();
    rotation.rotate(angle, rotationAxis);
    update();
}

void GLEngineWidget::mouseReleaseEvent(QMouseEvent *e)
{

}

void GLEngineWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_F1:
        rotation.setToIdentity();
        translation.setToIdentity();
        translation.translate(0.0, 0.0, -5.0);
        break;
    case Qt::Key_A:
        translation.translate(0.1,0.0,0.0);
        break;
    case Qt::Key_W:
        translation.translate(0.0,0.0,0.1);
        break;
    case Qt::Key_S:
        translation.translate(0.0,0.0,-0.1);
        break;
    case Qt::Key_D:
        translation.translate(-0.1,0.0,0.0);
        break;
    case Qt::Key_Z:
        translation.translate(0.0,0.1,0.0);
        break;
    case Qt::Key_X:
        translation.translate(0.0,-0.1,0.0);
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
