#ifndef GLWIDGETLAB2_H
#define GLWIDGETLAB2_H

#include "glenginewidget.h"
#include <QWheelEvent>
#include <QOpenGLTexture>

class GLWidgetLab2 : public GLEngineWidget
{
public:
    explicit GLWidgetLab2(QWidget* parent);
    void initializeGL();
    void paintGL();

private:
    const int m_circlePointsCount;
    QOpenGLTexture *m_texture;
};

#endif // GLWIDGETLAB2_H
