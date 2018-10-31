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
#include <math.h>
#include "modelstorage.h"

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
    void initializeGL();
protected slots:
    virtual void processCoordinates();
protected:
    QOpenGLShaderProgram *m_qShaderProgram;
    QString m_positionAttr;
    QString m_texCoordAttr;
    QString m_normalsAttr;
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
    ModelStorage m_vertexStorage;
    QOpenGLTexture* m_cubeTexture;
    GLuint m_inverseViewNormalMatrixAttr;
    float m_decelerationCoefficient;
    float m_cameraSpeedCoefficient;
    qreal m_zNear;
    qreal m_zFar;
    qreal m_fov;
    QString m_texturePath;
    QVector<QString> m_cubeTexturesPath;
    float m_rotationSens;
};

#endif // GLWIDGET_H
