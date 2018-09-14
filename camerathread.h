#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include<QThread>
#include<ros/ros.h>
#include<sensor_msgs/Image.h>
#include<opencv2/opencv.hpp>
#include<image_transport/image_transport.h>
#include<cv_bridge/cv_bridge.h>
#include"common.h"

class CameraThread : public QThread
{
    Q_OBJECT
public:
    CameraThread();
    void setImgWidth(int);
    int getImgWidth();
    void setImgHeight(int);
    int getImgHeight();
    void setFrameData(cv::Mat);
    cv::Mat getFrameData();


private:
    void displayFrame();
    void callBack(const sensor_msgs::Image::ConstPtr &);

    int image_width;
    int image_height;
    cv::Mat mat;

protected:
    void run();
};

#endif // CAMERATHREAD_H
