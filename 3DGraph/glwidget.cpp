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
    //Массив из 3 векторов которые будут представлять 3 вершины
    static const GLfloat vertices[] = {
       -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
    };
    //Массив из 3 векторов которые будут представлять цвета вершин
    static const GLfloat colors[] = {
       1.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 1.0f,
    };

    //standard way
    m_shaderProgram = loadShaders();
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, colors);
    glEnableVertexAttribArray(1);

    //QOpenGLShaderProgram way
    /*
    initShaders();
    glVertexAttribPointer(m_positionAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colorAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);
    */
}

void GLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //standard way
    glUseProgram(m_shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);


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

void GLWidget::initShaders() {
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

std::string readFromFile(QString &path){
    QFile file(path);
    QString textFromFile;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(file.isOpen())
    {
        while(!file.atEnd())
        {
            textFromFile += QString::fromStdString(file.readLine().toStdString()); //Что за костыль?
        }
    }
    return textFromFile.toStdString();
}

GLuint GLWidget::loadShaders(){
    // создаем шейдеры
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // читаем вершинный шейдер из файла
    std::string VertexShaderCode = readFromFile(m_vshaderPath);
    // читаем фрагментный шейдер из файла
    std::string FragmentShaderCode = readFromFile(m_fshaderPath);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Компилируем вершинный шейдер
    qDebug() << "Compiling shader : " <<  m_vshaderPath;
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Устанавливаем параметры
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Компилируем фрагментный шейдер
    qDebug() << "Compiling shader : " <<  m_fshaderPath;
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Устанавливаем параметры
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Устанавливаем параметры
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    return ProgramID;
}
