#ifndef ROBOT_H
#define ROBOT_H

#include<QTimer>
#include<QImage>
#include<iostream>
#include<geometry_msgs/PoseWithCovarianceStamped.h>
#include<QThread>
#include"common.h"
#include"robotthread.h"

using std::cout;
using std::endl;

class Robot
{
public:
    static Robot* getInstance();
    ~Robot();

    // setter and getter
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
    void setIsRobotAvailable(bool);
    bool getIsRobotAvailable();

    void move(double);
    void setImg(QImage);
    int** getImg();
    void newObstacleScanned(int pathIndex);
    void setLocalMap(int**, int, int, int, int);
    bool isVertexVaild(int, int);
    vector<Point> findNewPath(int, int);
    void inspecting();

    static const double radius = 0.25;
    static const double scan_radius = 1.5;
    bool isRobotAvailable;
    RobotThread* thread;

private:
    Robot();
    void callBack(geometry_msgs::PoseWithCovarianceStampedConstPtr &);



    double robot_radius;
    //    double scan_radius;
    vector<Vertex> inspectingPathVec;
    vector<Vertex> curPathVec;
    vector<Vertex> originPathVec;
    vector<Vertex> pathVec;
    vector<Vertex> visitedVertexVec;
    vector<Vertex> unreachedVertexVec;
    vector<Point> tempPathVec;
    int prePathIndex;
    int pathIndex;
    int nextPathIndex;
    QTimer timer;
    Pose pose;
    QImage img;
    int **map;
    int map_width;
    int map_height;
    static Robot* robot;
    //    PathPlanning pathPlanning;


};

#endif // ROBOT_H
