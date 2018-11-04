#ifndef GLWIDGETLAB2_H
#define GLWIDGETLAB2_H

#include "glenginewidget.h"

class GLWidgetLab : public GLEngineWidget
{
public:
    explicit GLWidgetLab(QWidget* parent);
    void paintGL();
    void calcVertices();
    void calcSphere();
protected:
    virtual void processCoordinates();
private:
    QVector<QVector4D> sphereVertices;
};

#endif // GLWIDGETLAB2_H
