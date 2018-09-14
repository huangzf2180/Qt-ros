#include "camera.h"


Camera::Camera(QWidget *parent) : QDialog(parent)
{
    setMinimumSize(500, 500);
    cameraThread = new CameraThread();
    cameraThread->start();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(displayFrame()));
    timer->start(33);
}

void Camera::displayFrame(){
    mat = cameraThread->getFrameData();
    cur_frame = cvMat2QImage(mat);
    update();
}

QImage Camera::cvMat2QImage(cv::Mat mat){
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

void Camera::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(0, 0, cur_frame.width(), cur_frame.height(), (QPixmap::fromImage(cur_frame)).scaled(cur_frame.width() * 2, cur_frame.height() * 2, Qt::KeepAspectRatio));
}
