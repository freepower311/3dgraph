#ifndef GLWIDGETLAB2_H
#define GLWIDGETLAB2_H

#include "glwidget.h"
#include <QWheelEvent>

class GLWidgetLab2 : public GLWidget
{
public:
    explicit GLWidgetLab2(QWidget* parent);
    void initializeGL();
    void paintGL();
private slots:
    void wheelEvent(QWheelEvent *e);

private:
    const int m_circlePointsCount;
};

#endif // GLWIDGETLAB2_H
