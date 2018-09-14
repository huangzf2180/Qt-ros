#include "robot.h"
#include<QTimer>
#include<iostream>
#include<unistd.h>


Robot* Robot::robot = NULL;

Robot::Robot()
{
    robot_radius = 0.25;/*
    scan_radius = 3;*/
    //    timer = new QTimer(this);
    //    vertexIndex = 0;
    thread = new RobotThread();
    thread->start();
}

Robot* Robot::getInstance(){
    if(robot == NULL){
        robot = new Robot();
    }
    return robot;
}

void Robot::setPose(Pose pose){
    this->pose.x = pose.x;
    this->pose.y = pose.y;
    this->pose.yaw = pose.yaw;
}

Pose Robot::getPose(){
    return pose;
}

void Robot::setPath(vector<Vertex> vec){
    pathVec.assign(vec.begin(), vec.end());
}

vector<Vertex> Robot::getPath(){
    return pathVec;
}

void Robot::setpathIndex(int index){
    pathIndex = index;
}

int Robot::getpathIndex(){
    return pathIndex;
}

void Robot::setvisitedVec(vector<Vertex> vec){
    visitedVertexVec = vec;
}

vector<Vertex> Robot::getvisitedVec(){
    return visitedVertexVec;
}

void Robot::setunreachedVertexVec(vector<Vertex> vec){
    unreachedVertexVec = vec;
}

vector<Vertex> Robot::getunreachedVertexVec(){
    return unreachedVertexVec;
}

void Robot::setIsRobotAvailable(bool flag){
    isRobotAvailable = flag;
}

bool Robot::getIsRobotAvailable(){
    return isRobotAvailable;
}

void Robot::setImg(QImage img){
    //    cout << "setImg" << endl;

    this->img = img;
    int width = img.width();
    int height = img.height();
    map_width = width;
    map_height = height;
    map = getVaildMap(img);
    Rect rect = getVaildMapRect(map, map_width, map_height);
    //    cout << "setImg end" << endl;
}

void Robot:: setLocalMap(int **localMap, int left, int top, int width, int height){
    for(int i=left; i<left+width;i++){
        for(int j=top; j<top+height; j++){
            map[i][j] = localMap[i-left][j-top];
        }
    }

    //    cout << "update localMap success" << endl;
    for(int i=left; i<left+width;i++){
        for(int j=top; j<top+height; j++){
            if (map[i][j] == 0)
            {
                for (int x = i - ROBOT_RADIUS >= 0 ? i - ROBOT_RADIUS : 0; x <= (i + ROBOT_RADIUS < map_width ? i + ROBOT_RADIUS : map_width - 1); x++)
                {
                    for (int y = j - ROBOT_RADIUS >= 0 ? j - ROBOT_RADIUS : 0; y <= (j + ROBOT_RADIUS < map_height ? j + ROBOT_RADIUS : map_height - 1); y++)
                    {
                        if (map[x][y] == 255)
                        {
                            int distance = pow(x - i, 2) + pow(y - j, 2);
                            if (distance <= ROBOT_RADIUS * ROBOT_RADIUS)
                            {
                                map[x][y] = 5;
                            }
                        }
                    }
                }
            }
        }
    }

}

int** Robot::getImg(){
    return map;
}

void Robot::newObstacleScanned(int pathIndex){
    for(int k = pathIndex; k < pathIndex + 5; k++){
        int x = pathVec.at(k).point.x;
        int y = pathVec.at(k).point.y;
        for(int i = x - Robot::scan_radius; i <= x + Robot::scan_radius; i++){
            for(int j = y - Robot::scan_radius; j <= y + Robot::scan_radius; j++){
//                if(pow(i - x, 2) + pow(j - y, 2) < Robot::radius * Robot::radius && qGray(img.pixel(i, j)) < 5);
            }
        }
    }
}

bool Robot::isVertexVaild(int x, int y){
    //    cout << "isVertexVaild: x = " << x << "y = " << y << endl;
    for(int i = x - ROBOT_RADIUS / 2; i <= x + ROBOT_RADIUS / 2; i++){
        for(int j = y - ROBOT_RADIUS / 2; j <= y + ROBOT_RADIUS / 2; j++){
            //            cout << (map[i][j] == 255 ? 1 : map[i][j]);
            if(pow(i - x, 2) + pow(j - y, 2) < ROBOT_RADIUS * ROBOT_RADIUS / 4 && (map[i][j] == 5 || map[i][j] == 0))
                return false;
        }
        //        cout << endl;
    }
    return true;
}

vector<Point> Robot::findNewPath(int x, int y){
    vector<Point> pointVec;
    vector<Vertex> vec;
    Point startPoint(x, y);
    for(int i = pathIndex + 1; i < (pathIndex + 20 < pathVec.size() ? pathIndex + 20 : pathVec.size()); i++){
        int x = pathVec.at(i).point.x;
        int y = pathVec.at(i).point.y;
        if(isVertexVaild(x, y)){
            pointVec = AStar(map, map_width, map_height, startPoint, Point(x, y));
            if(pointVec.size() == 0)
                break;
            this->pathIndex = i;
            for(vector<Vertex>::iterator it = vec.begin(); it != vec.end(); it++){
                visitedVertexVec.push_back(*it);
            }
            return pointVec;
        }
        else {
            vec.push_back(pathVec.at(i));
        }
    }

    for(vector<Vertex>::iterator it = vec.begin(); it != vec.end(); it++){
        unreachedVertexVec.push_back(*it);
    }

    return pointVec;
}

void Robot::move(double){
    //    cout << "move" << endl;
    Pose pose;
    int cur_x = getPose().x;
    int cur_y = getPose().y;
    int next_x;
    int next_y;
    Vertex v = pathVec.at(pathIndex);
    if(tempPathVec.size() > 0){
        next_x = tempPathVec.at(0).x;
        next_y = tempPathVec.at(0).y;
    }else{
        next_x = v.point.x;
        next_y = v.point.y;
    }
    if(!isVertexVaild(next_x, next_y)){
        cout << "vertex is not vaild" << endl;
        if(tempPathVec.size() == 0)
            unreachedVertexVec.push_back(v);
        tempPathVec.clear();
        tempPathVec = findNewPath(cur_x, cur_y);
        if(tempPathVec.size() == 0)
        {
            tempPathVec = AStar(map, map_width, map_height, Point(cur_x, cur_y), Point(pathVec.at(prePathIndex).point));
            pathVec = rebuildPath(unreachedVertexVec, visitedVertexVec, pathVec.at(prePathIndex));
            pathIndex = 0;
            if(pathVec.size() == 0)
                return;
            v = pathVec.at(pathIndex);
        }
    }

    if(tempPathVec.size() > 0){
        pose.x = tempPathVec.at(0).x;
        pose.y = tempPathVec.at(0).y;
        pose.yaw = 0;
        tempPathVec.erase(tempPathVec.begin(), tempPathVec.begin() + 10 < tempPathVec.end() ? tempPathVec.begin() + 10 : tempPathVec.end());
    }
    else{
        pose.x = v.point.x;
        pose.y = v.point.y;
        pose.yaw = 0;
        visitedVertexVec.push_back(v);
        prePathIndex = pathIndex;
        if(pathIndex < pathVec.size())
            pathIndex++;
    }
    //    cout << "pose: " << pose.x << " " << pose.y << endl;
    setPose(pose);
}

void Robot::inspecting(){
    if(!isRobotAvailable)
        return;

//    inspectingPathVec = pathPlanning.getOriPath();
//    pathVec = inspectingPathVec;
}

