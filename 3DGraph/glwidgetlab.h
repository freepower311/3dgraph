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
    QVector<QVector4D> m_sphereVertices;
    int m_spheresPerClaster;
    int m_spheresArcPointCount;
    int m_spheresArcsCount;
    QVector<QVector3D> m_spheresClastersCenters;
    float m_spheresDispersionXZ;
    float m_spheresDispersionY;
    float m_spheresScale;
};

#endif // GLWIDGETLAB2_H
