#include "obstacle.h"

Obstacle::Obstacle()
{
    isSelected = false;
}

void Obstacle::set_startPos(QPointF pos){
    startPos = pos;
}

void Obstacle::set_endPos(QPointF pos){
    endPos = pos;
}

void Obstacle::set_Shape(Shape s){
    shape = s;
}

QPointF Obstacle::get_startPos(){
    return startPos;
}

QPointF Obstacle::get_endPos(){
    return endPos;
}

Shape Obstacle::get_shape(){
    return shape;
}

void Obstacle::setRect(QRectF rect){
    this->rect = rect;
}

QRectF Obstacle::getRect(){
    return rect;
}
