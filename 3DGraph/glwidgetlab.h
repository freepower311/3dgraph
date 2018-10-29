#ifndef GLWIDGETLAB2_H
#define GLWIDGETLAB2_H

#include "glenginewidget.h"
#include <QWheelEvent>
#include <QOpenGLTexture>

class GLWidgetLab : public GLEngineWidget
{
public:
    explicit GLWidgetLab(QWidget* parent);
    void initializeGL();
    void paintGL();
protected:
    virtual void processCoordinates();
};

#endif // GLWIDGETLAB2_H
