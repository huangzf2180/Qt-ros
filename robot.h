#ifndef ROBOT_H
#define ROBOT_H

#include<QTimer>
#include<QImage>
#include<iostream>
#include"common.h"

using std::cout;
using std::endl;

class Robot
{
public:
    Robot();
    ~Robot();
    void setPose(Pose);
    Pose getPose();
    void setPath(vector<Vertex>);
    vector<Vertex> getPath();
    void setpathIndex(int);
    int getpathIndex();
    void setvisitedVec(vector<Vertex>);
    vector<Vertex> getvisitedVec();
    void setunreachedVertexVec(vector<Vertex>);
    vector<Vertex> getunreachedVertexVec();
    void move(double);
    void setImg(QImage);
    int** getImg();
    void newObstacleScanned(int pathIndex);
    void setLocalMap(int**, int, int, int, int);
    bool isVertexVaild(int, int);
    vector<Point> findNewPath(int, int);

//    PaintArea *paintArea;

    static const double radius = 0.25;
    static const double scan_radius = 1.5;

private:
    double robot_radius;
//    double scan_radius;
    vector<Vertex> pathVec;
    vector<Vertex> visitedVertexVec;
    vector<Vertex> unreachedVertexVec;
    vector<Point> tempPathVec;
    int pathIndex;
    QTimer *timer;
    Pose pose;
    QImage img;
    int **map;
    int map_width;
    int map_height;


};

#endif // ROBOT_H
