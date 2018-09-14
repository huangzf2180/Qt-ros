    #ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include<QDialog>
#include<ros/ros.h>
#include<sensor_msgs/Image.h>
#include<QTimer>
#include<QPainter>
#include<opencv2/opencv.hpp>
#include"common.h"
#include"camerathread.h"



class Camera : public QDialog
{
    Q_OBJECT
public:
    explicit Camera(QWidget *parent = 0);

private:
    QImage cvMat2QImage(cv::Mat);

    QTimer* timer;
    CameraThread* cameraThread;
    cv::Mat mat;
    QImage cur_frame;

protected:
    void paintEvent(QPaintEvent*);

signals:

public slots:
    void displayFrame();
};

#endif // CAMERA_H
