#ifndef GLWIDGETLAB1_H
#define GLWIDGETLAB1_H
#include "glwidget.h"

class GLWidgetLab1 : public GLWidget
{
public:
    explicit GLWidgetLab1(QWidget* parent);
    void initializeGL();
    void paintGL();

private:
    const int m_circlePointsCount;
};

#endif // GLWIDGETLAB1_H
