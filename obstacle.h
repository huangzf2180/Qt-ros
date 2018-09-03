#ifndef OBSTACLE_H
#define OBSTACLE_H

#include<QPointF>
#include<QRectF>
#include"common.h"


class Obstacle
{
public:

    Obstacle();
    void set_Shape(Shape);
    void set_startPos(QPointF);
    void set_endPos(QPointF);
    QPointF get_startPos();
    QPointF get_endPos();
    void setRect(QRectF);
    QRectF getRect();
    Shape get_shape();
    bool isSelected;
    QRectF rect;


private:
    QPointF startPos, endPos;
    Shape shape;
};

#endif // OBSTACLE_H
