#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QDebug>
#include <QVector>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QQuaternion>

class GLEngineWidget : public QOpenGLWidget, public QOpenGLFunctions
{
public:
    explicit GLEngineWidget(QWidget* parent);
    void resetMatrix();

protected:
    void loadShaders();
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void resizeGL(int w, int h);
    void initTextures();

    GLuint m_shaderProgram;
    GLuint m_positionAttr;
    GLuint m_colorAttr;
    GLuint m_texCoordAttr;
    QVector<GLfloat> m_vertexArray;
    QVector<GLfloat> m_colorArray;
    QVector<GLfloat> m_textureCoordinates;
    QString m_vshaderPath;
    QString m_fshaderPath;

    GLuint m_matrixAttr;

    QOpenGLTexture *texture;
    QMatrix4x4 projection;
    QMatrix4x4 rotation;
    QMatrix4x4 translation;
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
};

#endif // GLWIDGET_H
