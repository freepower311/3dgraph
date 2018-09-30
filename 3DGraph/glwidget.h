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
    void initializeGL();
    void paintGL();

protected:
    void initShaders();
    GLuint loadShaders();

    QOpenGLShaderProgram *m_qShaderProgram;
    GLuint m_shaderProgram;
    GLuint m_positionAttr;
    GLuint m_colorAttr;
    QVector<GLfloat> m_vertexArray;
    QVector<GLfloat> m_colorArray;
private:
    QString m_vshaderPath;
    QString m_fshaderPath;
};

#endif // GLWIDGET_H
