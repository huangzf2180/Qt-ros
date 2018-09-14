#ifndef ROBOTTHREAD_H
#define ROBOTTHREAD_H

#include<QThread>
#include<ros/ros.h>
#include<geometry_msgs/PoseWithCovarianceStamped.h>
#include"common.h"

class RobotThread : public QThread
{
    Q_OBJECT
public:
    RobotThread();
    void setRobotPose(Pose);
    Pose getRobotPose(RotateMat);

private:
    void callBack(const geometry_msgs::PoseWithCovarianceStampedConstPtr &);

    Pose pose;
    double posX;
    double posY;

protected:
    void run();

};

#endif // ROBOTTHREAD_H
