#ifndef COMMON_H
#define COMMON_H

#include<QImage>
#include<vector>
#include<qmath.h>
#include<algorithm>
#include<iostream>

#define SCAN_PIXELS_RADIUS 30
#define ROBOT_RADIUS 5
#define SCAN_PIXELS_RADIUS_OUT 10
#define RESOLUTION 0.1
#define OBSTACLE_COLOR 0
#define FREE_COLOR 255

using std::cout;
using std::endl;
using std::vector;

enum Shape{Ellipse, Rectangle, Line};
enum Object{Object_Obstacle, Object_Robot, Object_NULL};

struct Pose
{
    double x;
    double y;
    double yaw;
};

struct Point{

    Point(){}

    Point(int x, int y){
        this->x = x;
        this->y = y;
    }
    int x;
    int y;
};

struct ObstacleScanned
{
    int id;
    vector<Point> pointVec;
};

struct Vertex
{
    int id;
    Point point;
};

struct Edge{
    Point startPos;
    Point endPos;
    vector<Point> pointVec;
};

struct Rect{
    Rect(int left, int top, int right, int bottom){
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }
    int left;
    int top;
    int right;
    int bottom;
};

struct AStarData
{
    Point point;
    int g;
    int h;
    int f;
    AStarData *parent;
};

int **getOriginMap(QImage img);
int **getVaildMap(QImage img);
Rect getVaildMapRect(int **map, int width, int height);
vector<Point> AStar(int **map, int width, int height, Point start, Point end);
vector<Vertex> rebuildPath(vector<Vertex> unreachedVertexVec, vector<Vertex> visitedVertexVec, Vertex cur_vertex);
vector<int> makePathByAnt(double** graph, bool vis[], int curIndex, int count);

#endif // COMMON_H
