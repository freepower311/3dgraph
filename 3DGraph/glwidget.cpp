#include "glwidget.h"
#include <QFile>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
}

void GLWidget::initShaders()
{
    //compile shaders
    m_qShaderProgram = new QOpenGLShaderProgram(this);
    m_qShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, m_vshaderPath);
    m_qShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, m_fshaderPath);
    if (!m_qShaderProgram->link())
    {
        qWarning("Error: unable to link a shader program");
        return;
    }
    m_positionAttr = m_qShaderProgram->attributeLocation("positionAttr");
    m_colorAttr = m_qShaderProgram->attributeLocation("colorAttr");
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

void GLWidget::loadShaders()
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
}
