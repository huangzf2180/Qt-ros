#ifndef COMMON_H
#define COMMON_H

#include<QImage>
#include<vector>
#include<qmath.h>
#include<algorithm>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<string>
#include<ros/ros.h>


#define SCAN_PIXELS_RADIUS 30
#define ROBOT_RADIUS 5
#define SCAN_RADIUS 1.5
#define SCAN_PIXELS_RADIUS_OUT 10
#define RESOLUTION 0.05
#define OBSTACLE_COLOR 0
#define FREE_COLOR 255
#define PI 3.14159265


using cv::Mat;
using std::cout;
using std::endl;
using std::vector;
using std::string;

enum Shape{Ellipse, Rectangle, Line};
enum Object{Object_Obstacle, Object_Robot, Object_NULL};

struct Pose
{
    Pose(){};
    Pose(double x, double y, double yaw){
        this->x = x;
        this->y = y;
        this->yaw = yaw;
    }
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

struct RotateMat
{
    RotateMat() {}
    Mat mat;
    Mat region_mat;
    cv::Rect region_rect;
    int angle;
    int left;
    int top;
};


int **getOriginMap(QImage img);
int **getVaildMap(QImage img);
Rect getVaildMapRect(int **map, int width, int height);
vector<Point> AStar(int **map, int width, int height, Point start, Point end);
vector<Vertex> rebuildPath(vector<Vertex> unreachedVertexVec, vector<Vertex> visitedVertexVec, Vertex cur_vertex);
vector<int> makePathByAnt(double** graph, bool vis[], int curIndex, int count);
vector<Point> getObstacleScanPath(QImage img);
void getCurPathVertex(int start, int end, int **middle_vertex, vector<Vertex> &pathVec, vector<Vertex> &vertexVec);
RotateMat getMat(QString);
QImage cvMat2QImage(cv::Mat mat);
Pose getPose(QPointF point, RotateMat);

#endif // COMMON_H
