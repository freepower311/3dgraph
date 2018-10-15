#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QDebug>
#include <QVector>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QQuaternion>

const float PI =3.141592653;

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
    void resetCamera();

    GLuint m_shaderProgram;
    GLuint m_positionAttr;
    GLuint m_colorAttr;
    GLuint m_texCoordAttr;
    GLuint m_matrixAttr;
    QVector<GLfloat> m_vertexArray;
    QVector<GLfloat> m_colorArray;
    QVector<GLfloat> m_textureCoordinates;
    QString m_vshaderPath;
    QString m_fshaderPath;
    QOpenGLTexture *texture;
    QMatrix4x4 projection;
    QVector2D mousePressPosition;
    QVector3D eye;
    QVector3D center;
    QVector3D up;
    double cameraX;
    double cameraY;
    double cameraZ;
    double cameraAngleX;
    double cameraAngleY;
};

#endif // GLWIDGET_H
