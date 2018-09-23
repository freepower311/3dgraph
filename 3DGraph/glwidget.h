#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget> //or #include <QtOpenGL/QGLWidget>
#include <QOpenGLShaderProgram>

class GLWidget : public QOpenGLWidget, public QOpenGLFunctions
{
public:
    explicit GLWidget(QWidget* parent);
    void initializeGL();
    void paintGL();

protected:
    void initShaders();

    QOpenGLShaderProgram *m_program;
    GLuint pointerToTime;
    int timeCounter;
};

#endif // GLWIDGET_H
