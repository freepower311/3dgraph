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

const float PI = 3.141592653;

class GLEngineWidget : public QOpenGLWidget, public QOpenGLFunctions
{
public:
    explicit GLEngineWidget(QWidget* parent);

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
    GLuint m_texCoordAttr;
    GLuint m_matrixAttr;
    QVector<GLfloat> m_vertexArray;
    QVector<GLfloat> m_textureCoordinates;
    QString m_vshaderPath;
    QString m_fshaderPath;
    QOpenGLTexture *m_texture;
    QMatrix4x4 m_projection;
    QVector2D m_mousePressPosition;
    QVector3D m_eye;
    QVector3D m_center;
    QVector3D m_up;
    float m_cameraX;
    float m_cameraY;
    float m_cameraZ;
    float m_cameraAngleX;
    float m_cameraAngleY;
};

#endif // GLWIDGET_H
