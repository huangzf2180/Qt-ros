#include<iostream>
#include<QImage>
#include"common.h"

using std::cout;
using std::endl;


int **getOriginMap(QImage img){
    int width = img.width();
    int height = img.height();
    int **map;
    map = new int*[width];
    for(int i=0;i<width;i++){
        map[i] = new int[height];
    }
    for (int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++){
            QRgb rgb = img.pixel(i, j);
            map[i][j] = qGray(rgb);
        }
    }

    return map;
}

int **getVaildMap(QImage img){
    int width = img.width();
    int height = img.height();
    int **map;
    map = getOriginMap(img);
    Rect rect = getVaildMapRect(map, width, height);
    int left = rect.left;
    int top = rect.top;
    int right = rect.right;
    int bottom = rect.bottom;
    for (int i = left; i <= right; i++)
    {
        for (int j = top; j <= bottom; j++)
        {
            if (map[i][j] == 0)
            {
                for (int x = i - ROBOT_RADIUS >= 0 ? i - ROBOT_RADIUS : 0; x <= (i + ROBOT_RADIUS < width ? i + ROBOT_RADIUS : width - 1); x++)
                {
                    for (int y = j - ROBOT_RADIUS >= 0 ? j - ROBOT_RADIUS : 0; y <= (j + ROBOT_RADIUS < height ? j + ROBOT_RADIUS : height - 1); y++)
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

    return map;
}

Rect getVaildMapRect(int **map, int width, int height){
    int left = 10e9, top = 10e9, right = -1, bottom = -1;
    for (int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++){
            if(map[i][j] == 0){
                if (left > i)
                    left = i;
                if (top > j)
                    top = j;
                if (right < i)
                    right = i;
                if (bottom < j)
                    bottom = j;
            }
        }
    }
    return Rect(left, top, right, bottom);
}

void addData(int **map, int x, int y, Point end, AStarData *parent, vector<AStarData*> &open, bool **close){
    if(map[x][y] == 255){
        if(!close[x][y]){
            for(int i = x - ROBOT_RADIUS / 2; i <= x + ROBOT_RADIUS / 2; i++){
                for(int j = y - ROBOT_RADIUS / 2; j <= y + ROBOT_RADIUS / 2; j++){
                    if(map[i][j] == 5 && pow(i - x, 2) + pow(j - y, 2) <= ROBOT_RADIUS * ROBOT_RADIUS / 4)
                        return;
                }
            }
            AStarData *data = new AStarData();
            data->g = parent->g + 1;
            data->h = abs(x - end.x) + abs(y - end.y);
            data->f = data->g + data->h;
            data->point.x = x;
            data->point.y = y;
            data->parent = parent;
            open.push_back(data);
            close[x][y] = true;
        }
    }
}

vector<Point> AStar(int **map, int width, int height, Point start, Point end){
    int start_x = start.x;
    int start_y = start.y;
    int end_x = end.x;
    int end_y = end.y;
    vector<Point> pointVec;
    vector<AStarData*> open;
    bool **close;
    close = new bool*[width];
    for(int i = 0; i < width; i++){
        close[i] = new bool[height];
    }
    memset(close, false, sizeof(close));
    AStarData *endData;
    AStarData* data = new AStarData();
    data->point.x = start_x;
    data->point.y = start_y;
    data->g = 0;
    data->h = abs(start_x - end_x) + abs(start_y - end_y);
    data->f = data->g + data->h;
    data->parent = NULL;
    open.push_back(data);
    while(true){
        int min_f = 10e9;
        int nextDataIndex = -1;
        for(int i=0;i<open.size();i++){
            if(open.at(i)->f <= min_f){
                min_f = open.at(i)->f;
                nextDataIndex = i;
            }
        }
        AStarData *data = open.at(nextDataIndex);
        int x = data->point.x;
        int y = data->point.y;
        int g = data->g;
        if(x == end_x && y == end_y){
            endData = data;
            break;
        }
        open.erase(open.begin() + nextDataIndex);
        close[x][y] = true;
        addData(map, x - 1, y, end, data, open, close);
        addData(map, x, y + 1, end, data, open, close);
        addData(map, x + 1, y, end, data, open, close);
        addData(map, x, y - 1, end, data, open, close);
        if(open.size() == 0 || data->g > 200){
            endData = NULL;
            break;
        }
    }

    if(endData){
        while(true){
            Point point = endData->point;
            pointVec.push_back(point);
            if(endData->parent != NULL){
                endData = endData->parent;
            }
            else{
                break;
            }
        }
    }

    std::reverse(pointVec.begin(), pointVec.end());

    return pointVec;
}

int getMaxEdgeLen(Mat mat)
{
    int max_len = -1;
    for (int i = 0; i < mat.rows; i++)
    {
        uchar *p = mat.ptr<uchar>(i);
        int maxRowLen = 0;
        for (int j = 0; j < mat.cols; j++)
        {
            if (p[j] == 0)
                maxRowLen++;
            else
            {
                if (max_len < maxRowLen)
                {
                    max_len = maxRowLen;
                    maxRowLen = 0;
                }
            }
        }
        if (max_len < maxRowLen)
        {
            max_len = maxRowLen;
        }
    }

    return max_len;
}

RotateMat getRotationMat(Mat mat, int width, int height)
{
    cv::Point2d center(width / 2, height / 2);
    cv::Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
    int maxEdgeLen = -1;
    RotateMat result;
    for (int angle = 0; angle < 180; angle++)
    {
        double sinAngle = sin(angle * PI / 180);
        double cosAngle = cos(angle * PI / 180);
        cv::Point2d leftTop(-center.x, center.y);
        cv::Point2d rightTop(mat.cols - center.x, center.y);
        cv::Point2d leftBottom(-center.x, -mat.rows + center.y);
        cv::Point2d rightBottom(mat.cols - center.x, -mat.rows + center.y);
        transLeftTop = cv::Point2d(leftTop.x * cosAngle + leftTop.y * sinAngle, -leftTop.x * sinAngle + leftTop.y * cosAngle);
        transRightTop = cv::Point2d(rightTop.x * cosAngle + rightTop.y * sinAngle, -rightTop.x * sinAngle + rightTop.y * cosAngle);
        transLeftBottom = cv::Point2d(leftBottom.x * cosAngle + leftBottom.y * sinAngle, -leftBottom.x * sinAngle + leftBottom.y * cosAngle);
        transRightBottom = cv::Point2d(rightBottom.x * cosAngle + rightBottom.y * sinAngle, -rightBottom.x * sinAngle + rightBottom.y * cosAngle);
        double left = std::min(transLeftTop.x, transRightTop.x);
        left = std::min(left, transLeftBottom.x);
        left = std::min(left, transRightBottom.x);
        double right = std::max(transLeftTop.x, transRightTop.x);
        right = std::max(right, transLeftBottom.x);
        right = std::max(right, transRightBottom.x);
        double top = std::max(transLeftTop.y, transRightTop.y);
        top = std::max(top, transLeftBottom.y);
        top = std::max(top, transRightBottom.y);
        double down = std::min(transLeftTop.y, transRightTop.y);
        down = std::min(down, transLeftBottom.y);
        down = std::min(down, transRightBottom.y);
        int dst_width = static_cast<int>(abs(left - right) + 0.5);
        int dst_height = static_cast<int>(abs(top - down) + 0.5);

        Mat dst_mat(dst_height, dst_width, 0);
        double dx = left * cosAngle - top * sinAngle + center.x;
        double dy = -left * sinAngle - top * cosAngle + center.y;

        uchar *p;
        for (int i = 0; i < dst_height; i++)
        {
            p = dst_mat.ptr<uchar>(i);
            for (int j = 0; j < dst_width; j++)
            {
                int x = static_cast<int>(j * cosAngle + i * sinAngle + dx + 0.5);
                int y = static_cast<int>(-j * sinAngle + i * cosAngle + dy + 0.5);

                if (x >= 0 && y >= 0 && x < mat.cols && y < mat.rows)
                    p[j] = mat.ptr<uchar>(y)[x];
                else
                    p[j] = 205;
            }
        }

        int len = getMaxEdgeLen(dst_mat);
        if (maxEdgeLen < len)
        {
            maxEdgeLen = len;
            result.mat = dst_mat;
            result.angle = angle;
            result.left = left;
            result.top = top;
        }
    }

    return result;
}

RotateMat getMat(QString path){
    cv::Mat ori_mat;
    ori_mat = cv::imread(path.toStdString(), 0);
    int left = INT_MAX, top = INT_MAX, right = -1, bottom = -1;
    for (int i = 0; i < ori_mat.rows; i++)
    {
        uchar *p = ori_mat.ptr<uchar>(i);
        for (int j = 0; j < ori_mat.cols; j++)
        {
            if (p[j] == 0)
            {
                if (left > j)
                    left = j;
                if (top > i)
                    top = i;
                if (right < j)
                    right = j;
                if (bottom < i)
                    bottom = i;
            }
        }
    }

    int width = right - left + 10;
    int height = bottom - top + 10;
    cv::Mat mat(ori_mat, cv::Rect(left - 5, top - 5, width, height));
    RotateMat rotateMat = getRotationMat(mat, width, height);
    rotateMat.region_mat = mat;
    rotateMat.region_rect = cv::Rect(left - 5, top - 5, width, height);
    return rotateMat;
}

QImage cvMat2QImage(cv::Mat mat){
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

Pose getPose(QPointF point, RotateMat rotateMat){
    int x = point.x() / 0.05;
    int y = point.y() / 0.05;

    Mat mat = rotateMat.region_mat;

    int width = mat.cols;
    int height = mat.rows;

    x = 1000 + x - rotateMat.region_rect.x;
    y = 984 - y - rotateMat.region_rect.y;

    int angle = rotateMat.angle;

    cv::Point2d center(width / 2, height / 2);
    cv::Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;
    double sinAngle = sin(angle * PI / 180);
    double cosAngle = cos(angle * PI / 180);
    cv::Point2d leftTop(-center.x, center.y);
    cv::Point2d rightTop(mat.cols - center.x, center.y);
    cv::Point2d leftBottom(-center.x, -mat.rows + center.y);
    cv::Point2d rightBottom(mat.cols - center.x, -mat.rows + center.y);
    transLeftTop = cv::Point2d(leftTop.x * cosAngle + leftTop.y * sinAngle, -leftTop.x * sinAngle + leftTop.y * cosAngle);
    transRightTop = cv::Point2d(rightTop.x * cosAngle + rightTop.y * sinAngle, -rightTop.x * sinAngle + rightTop.y * cosAngle);
    transLeftBottom = cv::Point2d(leftBottom.x * cosAngle + leftBottom.y * sinAngle, -leftBottom.x * sinAngle + leftBottom.y * cosAngle);
    transRightBottom = cv::Point2d(rightBottom.x * cosAngle + rightBottom.y * sinAngle, -rightBottom.x * sinAngle + rightBottom.y * cosAngle);
    double left = std::min(transLeftTop.x, transRightTop.x);
    left = std::min(left, transLeftBottom.x);
    left = std::min(left, transRightBottom.x);
    double right = std::max(transLeftTop.x, transRightTop.x);
    right = std::max(right, transLeftBottom.x);
    right = std::max(right, transRightBottom.x);
    double top = std::max(transLeftTop.y, transRightTop.y);
    top = std::max(top, transLeftBottom.y);
    top = std::max(top, transRightBottom.y);
    double down = std::min(transLeftTop.y, transRightTop.y);
    down = std::min(down, transLeftBottom.y);
    down = std::min(down, transRightBottom.y);

    x = x - center.x;
    y = -y + center.y;
    int x1 = x * cosAngle + y * sinAngle;
    int y1 = -x * sinAngle + y * cosAngle;
    int x2 = x1 - left;
    int y2 = top - y1;

    return Pose(x2, y2, 0);
}
