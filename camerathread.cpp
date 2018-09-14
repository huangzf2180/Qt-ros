#include "camerathread.h"
#include<image_transport/image_transport.h>

CameraThread::CameraThread()
{

}


void CameraThread::callBack(const sensor_msgs::Image::ConstPtr &msg){
    image_width = msg->width;
    image_height = msg->height;
    mat = cv_bridge::toCvShare(msg, "bgr8")->image;
}

void CameraThread::setImgWidth(int width){
    image_width = width;
}

int CameraThread::getImgWidth(){
    return image_width;
}

void CameraThread::setImgHeight(int height){
    image_height = height;
}

int CameraThread::getImgHeight(){
    return image_height;
}

void CameraThread::setFrameData(cv::Mat mat){
    this->mat = mat;
}

cv::Mat CameraThread::getFrameData(){
    return mat;
}

void CameraThread::run(){
    int argc = 0;
    char *argv[] = {};
    ros::init(argc, argv, "qt_camera");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber subscriber = it.subscribe("/image_raw", 1, &CameraThread::callBack, this);
    ros::Rate r(30);
    while(ros::ok()){
        ros::spinOnce();
        r.sleep();
    }
}
