#ifndef GLWIDGETLAB2_H
#define GLWIDGETLAB2_H

#include "glenginewidget.h"

class GLWidgetLab : public GLEngineWidget
{
public:
    explicit GLWidgetLab(QWidget* parent);
    void paintGL();
protected:
    virtual void processCoordinates();
};

#endif // GLWIDGETLAB2_H
