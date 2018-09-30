#include "glwidget.h"
#include <QFile>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent),
    m_vshaderPath(":/vshader.glsl"),
    m_fshaderPath(":/fshader.glsl")
{
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    m_vertexArray = {
        -0.9f, -0.9f, 0.0f,
        -0.5f, 0.0f, 0.0f,
        -0.1f, -0.9f, 0.0f,
        0.1f, 0.1f, 0.0f,
        0.1f, 0.4f, 0.0f,
        0.5f, 0.4f, 0.0f,
        0.5f, 0.1f, 0.0f
    };
    m_colorArray = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f
    };
    const unsigned int circlePoints = 20;
    for(int i = 0; i < circlePoints; i++)
    {
        m_vertexArray.append(-0.5 + 0.2*cos(i*2*M_PI/circlePoints));
        m_vertexArray.append(0.5 + 0.2*sin(i*2*M_PI/circlePoints));
        m_vertexArray.append(0.0);
    }
    for(int i = 0; i < circlePoints; i++)
    {
        m_colorArray.append(1.0);   //R
        m_colorArray.append(1.0);   //G
        m_colorArray.append(0.0);   //B
    }

    //standard way
    m_shaderProgram = loadShaders();
    m_positionAttr = 0;
    m_colorAttr = 1;

    //QOpenGLShaderProgram way
    //initShaders();

    glVertexAttribPointer(m_positionAttr, 3, GL_FLOAT, false, 0, m_vertexArray.data());
    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, false, 0, m_colorArray.data());

}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //standard way
    glUseProgram(m_shaderProgram);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_QUADS, 3, 4);
    glDrawArrays(GL_POLYGON, 7, 20);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glUseProgram(0);



    //QOpenGLShaderProgram way
    /*
    m_qShaderProgram->bind();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    m_qShaderProgram->release();
    */
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

GLuint GLWidget::loadShaders()
{
    //создаем шейдеры
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    //читаем вершинный шейдер из файла
    std::string vertexShaderCode = readFromFile(m_vshaderPath);
    //читаем фрагментный шейдер из файла
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
    fprintf(stdout, "%s\n", &vertexShaderErrorMessage[0]);

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
    fprintf(stdout, "%s\n", &fragmentShaderErrorMessage[0]);

    fprintf(stdout, "Linking program\n");
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    //Устанавливаем параметры
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programErrorMessage( std::max(infoLogLength, int(1)));
    glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
    fprintf(stdout, "%s\n", &programErrorMessage[0]);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    return programID;
}
