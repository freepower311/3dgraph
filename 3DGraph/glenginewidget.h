#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QDebug>
#include <QVector>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLShaderProgram>
#include "objloader.h"

const float PI = 3.141592653;

class GLEngineWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
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
protected slots:
    virtual void processCoordinates();
protected:
    QOpenGLShaderProgram *m_qShaderProgram;
    GLuint m_positionAttr;
    GLuint m_texCoordAttr;
    GLuint m_matrixAttr;
    GLuint m_normalsAttr;
    GLuint m_normalMatrixAttr;
    GLuint m_viewMatrixAttr;
    GLuint m_viewSpaceLightPosition;
    QVector<GLfloat> m_vertexArray;
    QVector<GLfloat> m_textureCoordinates;
    QVector<GLfloat> m_normalsArray;
    QString m_vshaderPath;
    QString m_fshaderPath;
    QOpenGLTexture *m_texture;
    QMatrix4x4 m_projection;
    QVector2D m_mousePressPosition;
    QVector3D m_eye;
    QVector3D m_center;
    QVector3D m_up;
    QTimer m_processCoordinatesTimer;
    QTimer m_drawSceneTimer;
    float m_cameraX;
    float m_cameraY;
    float m_cameraZ;
    float m_cameraAngleX;
    float m_cameraAngleY;
    float m_cameraXSpeed;
    float m_cameraYSpeed;
    float m_cameraZSpeed;
    QVector4D m_lightPosition;
    OBJLoader m_objLoader;
    QOpenGLTexture* m_cubeTexture;
    GLuint m_inverseViewNormalMatrixAttr;
};

#endif // GLWIDGET_H
