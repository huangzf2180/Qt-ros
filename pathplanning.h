#ifndef PATHPLANNING_H
#define PATHPLANNING_H

#include"common.h"
#include"map.h"

class PathPlanning
{
public:
    PathPlanning();
    ~PathPlanning();

    vector<Vertex> getOriPath();
    vector<Vertex> getCurPath();


private:


    Map* map;
    vector<Vertex> vertexVec;
    vector<Vertex> pathVec;

};

#endif // PATHPLANNING_H
