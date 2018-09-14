#include "robotthread.h"

RobotThread::RobotThread()
{
    setRobotPose(Pose(-2.0 ,-1.0 ,0.0));
}

void RobotThread::setRobotPose(Pose pose){
    this->pose = pose;
}

Pose RobotThread::getRobotPose(RotateMat rotateMat){
    return getPose(QPointF(pose.x, pose.y), rotateMat);
}

void RobotThread::callBack(const geometry_msgs::PoseWithCovarianceStampedConstPtr &msg){
    double x = msg->pose.pose.position.x;
    double y = msg->pose.pose.position.y;
    setRobotPose(Pose(x, y, 0));
//    Robot* robot = Robot::getInstance();
//    robot->setPose(Pose(x, y, 0));
}

void RobotThread::run(){
    ros::NodeHandle nh;
    ros::Subscriber subscriber = nh.subscribe("/amcl_pose", 1, &RobotThread::callBack, this);
    ros::Rate r(30);
    while(ros::ok()){
        ros::spinOnce();
        r.sleep();
    }
}
