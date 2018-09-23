#include "glwidget.h"

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    timeCounter = 0;
}

void GLWidget::initializeGL(){
    initializeOpenGLFunctions();
    initShaders();
}

void GLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT);
    timeCounter++;
    m_program->setUniformValue(pointerToTime, timeCounter);

    glBegin(GL_POINTS);
    for(int i = 0;i<10;i++){
        for(int j = 0;j<100;j++){
            glVertex3f(i*0.2 -1, j * 0.02 -1 + (timeCounter%100)/100, 0);
        }
    }
    for(int i = 0;i<100;i++){
        for(int j = 0;j<10;j++){
            glVertex3f(i*0.02 -1 + (timeCounter%100)/100, j * 0.2 -1,0);
        }
    }
    glEnd();
}

void GLWidget::initShaders() {
    const GLubyte *extensions;
    extensions = glGetString(GL_EXTENSIONS);
    int i = 0;
    QString extensionsString;
    while (extensions[i] != 0) {
        extensionsString += extensions[i];
        i++;
    }
    if (!extensionsString.contains("GL_ARB_vertex_shader") ||
            !extensionsString.contains("GL_ARB_fragment_shader") ||
            !extensionsString.contains("GL_ARB_shader_objects") ||
            !extensionsString.contains("GL_ARB_shading_language_100"))
    {
        exit(0);
    }

    //compile shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl");
    if (!m_program->link())
    {
        qWarning("Error: unable to link a shader program");
        return;
    }
    pointerToTime = m_program->uniformLocation( "time" );
    m_program->bind();
}
