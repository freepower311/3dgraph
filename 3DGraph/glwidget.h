#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QVector>

class GLWidget : public QOpenGLWidget, public QOpenGLFunctions
{
public:
    explicit GLWidget(QWidget* parent);

protected:
    void loadShaders();

    QOpenGLShaderProgram *m_qShaderProgram;
    GLuint m_shaderProgram;
    GLuint m_positionAttr;
    GLuint m_colorAttr;
    QVector<GLfloat> m_vertexArray;
    QVector<GLfloat> m_colorArray;
    QString m_vshaderPath;
    QString m_fshaderPath;
};

#endif // GLWIDGET_H
