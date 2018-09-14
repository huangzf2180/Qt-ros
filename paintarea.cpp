#include "paintarea.h"
#include<QPainter>
#include<iostream>
#include<cmath>
#include<cstdio>
#include<ctime>
#include<stdio.h>
#include<QFileDialog>
#include<QTimer>

using std::cout;
using std::endl;


double PaintArea::real_width = NULL;
double PaintArea::real_height = NULL;
double PaintArea::pixmap_width = 500;
double PaintArea::pixmap_height = 400;
double PaintArea::resolution = 0.05;

QPointF PaintArea::drawStartPos;

vector<Vertex> get_path(QImage img);

PaintArea::PaintArea(QWidget *parent) : QWidget(parent)
{

    setPalette(QPalette(Qt::transparent));
    setAutoFillBackground(true);
    setMinimumSize(pixmap_width * 2, pixmap_height * 2);
    setPen(QPen(Qt::black));
    //    map = Map::getInstance();
    //    map->setPixmap(&pixmap);
    isShapeSelected = false;
    robot = Robot::getInstance();
    modifyAction = new QAction(tr("modify"), this);
    connect(modifyAction, SIGNAL(triggered()), this, SLOT(showModifyDialog()));
    contextMenu.addAction(modifyAction);
    modifyDialog = NULL;
    scale = 1.8;
    drawPixmap(&pixmap, 1);
}

void PaintArea::init(){
}

void PaintArea::setShape(Shape s){
    shape = s;
}

void PaintArea::setPen(QPen p){
    pen = p;
}

void PaintArea::setBrush(QBrush b){
    brush = b;
    update();
}

void PaintArea::setFillRule(Qt::FillRule rule){
    fillRule = rule;
    update();
}

void PaintArea::setIsShapeSelected(bool isShapeSelected){
    this->isShapeSelected = isShapeSelected;
}

bool PaintArea::getIsShapeSelected(){
    return isShapeSelected;
}

void PaintArea::setIsObstacleSelected(bool isObstacleSelected){
    this->isObstacleSelected = isObstacleSelected;
}

bool PaintArea::getIsObstacleSelected(){
    return isObstacleSelected;
}

void PaintArea::showModifyDialog(){
    modifyDialog = new ModifyDialog(obstacleVec, obstacleSelectedIndexVec, this);
    int res = modifyDialog->exec();
    if(res == QDialog::Accepted){
        drawPixmap(&pixmap, 1);
    }
    delete modifyDialog;
}

bool PaintArea::isMouseInDrawArea(QPointF point){
    if(point.x() < drawStartPos.x() || point.x() > drawStartPos.x() + pixmap_width * scale || point.y() < drawStartPos.y() || point.y() > drawStartPos.y() + pixmap_height * scale){
        return false;
    }
    return true;
}

void PaintArea::mousePressEvent(QMouseEvent *e){
    startPos = e->posF();
    if(!isMouseInDrawArea(startPos)) return;
    moveStartPos = e->posF();
    if(e->button() == Qt::LeftButton){
        switch(putObject){
        case Object_Obstacle:{
            Obstacle obstacle;
            obstacle.set_Shape(shape);
            double x = (startPos.x() - drawStartPos.x()) / scale;
            double y = (startPos.y() - drawStartPos.y()) / scale;
            obstacle.setRect(QRectF(x, y, 0, 0));
            obstacleVec.push_back(obstacle);
        }
            break;
        case Object_Robot:
        {
            robot->setIsRobotAvailable(true);
            Pose pose;
            pose.x = (startPos.x() - drawStartPos.x()) / scale;
            pose.y = (startPos.y() - drawStartPos.y()) / scale;
            pose.yaw = 0;
            robot->setPose(pose);
        }
            break;
        default:
            if(isPointInPolygon(e->posF()) != -1){
                if(!obstacleVec.at(obstacleSelectedIndex).isSelected){
                    obstacleSelectedIndexVec.push_back(obstacleSelectedIndex);
                    obstacleVec.at(obstacleSelectedIndex).isSelected = true;
                    cout<<obstacleSelectedIndex<<endl;
                }
                setIsObstacleSelected(true);
            }
            else{
                for(int i = 0; i< obstacleVec.size(); i++){
                    obstacleVec.at(i).isSelected = false;
                }
                obstacleSelectedIndexVec.clear();
            }
            break;
        }
    }
    else if(e->button() == Qt::RightButton){
        for(int i = 0; i< obstacleVec.size(); i++){
            obstacleVec.at(i).isSelected = false;
        }
        obstacleSelectedIndexVec.clear();
        if(isPointInPolygon(e->posF()) != -1){
            obstacleSelectedIndexVec.push_back(obstacleSelectedIndex);
            obstacleVec.at(obstacleSelectedIndex).isSelected = true;
            setIsObstacleSelected(true);
        }
    }
    drawPixmap(&pixmap, 1);
}

void PaintArea::mouseMoveEvent(QMouseEvent *e){
    if(!isMouseInDrawArea(startPos)) return;
    moveEndPos = e->posF();
    endPos = e->posF();
    switch (putObject) {
    case Object_Obstacle:
    {
        endPos = e->posF();
        double width = endPos.x() - startPos.x();
        double height = endPos.y() - startPos.y();
        obstacleVec.at(obstacleVec.size() - 1).rect.setWidth(width / scale);
        obstacleVec.at(obstacleVec.size()-1).rect.setHeight(height / scale);
        drawPixmap(&pixmap, 1);
    }
        break;
    case Object_Robot:
    {
        double sub_x = endPos.x() - startPos.x();
        double sub_y = endPos.y() - startPos.y();
        double yaw = atan2(sub_y, sub_x);
        Pose pose;
        pose.x = (startPos.x() - drawStartPos.x()) / scale;
        pose.y = (startPos.y() - drawStartPos.y()) / scale;
        pose.yaw = yaw;
        robot->setPose(pose);
        drawPixmap(&pixmap, 1);
    }
        break;
    default:
        if(getIsObstacleSelected()){
            float distance_x = moveEndPos.x() - moveStartPos.x();
            float distance_y = moveEndPos.y() - moveStartPos.y();
            moveObstacle(distance_x, distance_y);
            drawPixmap(&pixmap, 1);
        }
        break;
    }

    moveStartPos = moveEndPos;
}

void PaintArea::mouseReleaseEvent(QMouseEvent *e){
    if(!isMouseInDrawArea(startPos)) return;
    endPos = e->posF();
    switch (putObject) {
    case Object_Obstacle:
    {
        if(fabs(startPos.x() - endPos.x()) < 0.01 && fabs(startPos.y() - endPos.y()) < 0.01){
            obstacleVec.pop_back();
        }
        else{
            mouseMoveEvent(e);
            double x = startPos.x() - drawStartPos.x();
            double y = startPos.y() - drawStartPos.y();
            double width = endPos.x() - startPos.x();
            double height = endPos.y() - startPos.y();
            obstacleVec.at(obstacleVec.size() - 1).setRect(QRectF(x / scale, y / scale, width / scale, height / scale));
        }
    }
        break;
    case Object_Robot:
    {
        double sub_x = endPos.x() - startPos.x();
        double sub_y = endPos.y() - startPos.y();
        double yaw = atan2(sub_y, sub_x);
        Pose pose;
        pose.x = (startPos.x() - drawStartPos.x()) / scale;
        pose.y = (startPos.y() - drawStartPos.y()) / scale;
        pose.yaw = yaw;
        robot->setPose(pose);
    }
        break;
    default:

        break;
    }

    drawPixmap(&pixmap, 1);

    putObject = Object_NULL;
}

void PaintArea::wheelEvent(QWheelEvent *e){
    if(pixmap.width() == 0 && pixmap.height() == 0) return;
    if(e->delta() > 0 && scale < 5.0){
        scale += 0.1;
        QSize size = this->geometry().size();
        drawStartPos.setX((size.width() - pixmap_width * scale) / 2);
        drawStartPos.setY((size.height() - pixmap_height * scale) / 2);
    }
    else if(e->delta() < 0 && scale > 0.2){
        scale -= 0.1;
        QSize size = this->geometry().size();
        drawStartPos.setX((size.width() - pixmap_width * scale) / 2);
        drawStartPos.setY((size.height() - pixmap_height * scale) / 2);
    }
    else{
        return;
    }

    drawPixmap(&pixmap, 1);
}

void PaintArea::contextMenuEvent(QContextMenuEvent *e){

    contextMenu.exec(QCursor::pos());
}

void PaintArea::resizeEvent(QResizeEvent *e){
    size = e->size();
    drawStartPos.setX((size.width() - pixmap_width * scale) / 2);
    drawStartPos.setY((size.height() - pixmap_height * scale) / 2);
}

void PaintArea::moveObstacle(float distance_x, float distance_y){
    for(int i = 0; i < obstacleSelectedIndexVec.size(); i++){
        int index = obstacleSelectedIndexVec.at(i);
        QRectF rect = obstacleVec.at(index).getRect();
        obstacleVec.at(index).setRect(QRectF(rect.x() + distance_x / scale, rect.y() + distance_y / scale, rect.width(), rect.height()));
    }
}

int PaintArea::isPointInPolygon(QPointF point){
    vector<int> obstacleIndexVec;
    for(int i = 0; i < obstacleVec.size(); i++){
        Shape shape = obstacleVec.at(i).get_shape();
        QRectF rect = obstacleVec.at(i).getRect();
        double startPosX = rect.x() * scale + drawStartPos.x();
        double startPosY = rect.y() * scale + drawStartPos.y();
        double endPosX = startPosX + rect.width() * scale;
        double endPosY = startPosY + rect.height() * scale;

        switch (shape) {
        case Ellipse:{
            float origin_x = (startPosX + endPosX) / 2;
            float origin_y = (startPosY + endPosY) / 2;
            if(pow(point.x() - origin_x, 2) / pow((endPosX - startPosX)/ 2, 2) + pow(point.y() - origin_y, 2) / pow((endPosY - startPosY) / 2, 2) <= 1){
                obstacleIndexVec.push_back(i);
            }
        }
            break;
        case Rectangle:{
            QPointF p_1(QPointF(startPosX, startPosY));
            QPointF p_2(QPointF(startPosX, endPosY));
            QPointF p_3(QPointF(endPosX, endPosY));
            QPointF p_4(QPointF(endPosX,startPosY));
            vector<QPointF> vec;
            vec.push_back(p_1);
            vec.push_back(p_2);
            vec.push_back(p_3);
            vec.push_back(p_4);
            int count = vec.size();
            bool oddNodes = false;
            for (int i = 0,j=count-1; i<count; i++) {
                if(((vec.at(i).y() < point.y() && vec.at(j).y()>=point.y()) || (vec.at(j).y() < point.y() && vec.at(i).y()>=point.y())) && (vec.at(i).x()>=point.x() || vec.at(j).x()>=point.x())){
                    if(vec.at(i).x()+(point.y()-vec.at(i).y()) / (vec.at(j).y()-vec.at(i).y())*(vec.at(j).x()-vec.at(i).x())>point.x()){
                        oddNodes=!oddNodes;
                    }
                }
                j=i;
            }
            if(oddNodes) obstacleIndexVec.push_back(i);
        }
            break;
        case Line:{
            if(startPosX - endPosX == 0){
                if(point.x() == startPosX)
                    if((point.y() >= startPosY && point.y() <= endPosY) || (point.y() <= startPosY && point.y() >= endPosY))
                        obstacleIndexVec.push_back(i);
            }
            else {
                double a =(startPosY - endPosY) / (startPosX - endPosX);
                double b = -1;
                double c = -a * startPosX + startPosY;
                double distance = fabs(a * point.x() + b * point.y() + c) / sqrt(pow(a, 2) + pow(b, 2));
                if(distance < 3) obstacleIndexVec.push_back(i);
            }
        }
            break;
        default:
            break;
        }
    }

    return obstacleSelect(obstacleIndexVec, point);
}

int PaintArea::obstacleSelect(vector<int> obstacleIndexVec, QPointF point){

    if(obstacleIndexVec.size() == 0) return -1;
    float min_distance = 99999;
    for(vector<int>::iterator it = obstacleIndexVec.begin(); it != obstacleIndexVec.end(); it++){
        Obstacle obstacle = obstacleVec.at(*it);
        QPointF startPos = obstacle.get_startPos();
        QPointF endPos = obstacle.get_endPos();
        Shape shape = obstacle.get_shape();
        float origin_x = (startPos.x() + endPos.x()) / 2;
        float origin_y = (startPos.y() + endPos.y()) / 2;
        float distance = sqrt(pow(point.x() - origin_x, 2) + pow(point.y() - origin_y, 2));
        if(distance < min_distance){
            min_distance = distance;
            obstacleSelectedIndex = *it;
        }
    }

    return obstacleSelectedIndex;
}

void PaintArea::drawMap(QPixmap *pixmap, double scale){
    //    cout << "drawMap" << endl;

    QPainter *painter = new QPainter(pixmap);
    //this property will change the border pixel color of the shape
    //    painter->setRenderHint(QPainter::Antialiasing, true);
    pen.setColor(qGray(0));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(QBrush(qGray(0)));
    //    painter->drawLine(0, 0, pixmap_width * scale, 0);
    //    painter->drawLine(0, 0, 0, pixmap_height * scale);
    //    painter->drawLine(0, pixmap_height * scale, pixmap_width * scale, pixmap_height * scale);
    //    painter->drawLine(pixmap_width * scale, 0, pixmap_width * scale, pixmap_height * scale);
    for(vector<Obstacle>::iterator it = obstacleVec.begin(); it != obstacleVec.end(); it++){
        if((*it).isSelected){
            pen.setColor(Qt::red);
            painter->setPen(pen);
        }
        else{
            pen.setColor(qGray(0));
            painter->setPen(pen);
        }

        QRectF rect = (*it).getRect();
        QRectF draw_rect = QRectF(rect.x() * scale, rect.y() * scale, rect.width() * scale, rect.height() * scale);
        Shape shape = (*it).get_shape();

        switch (shape) {
        case Ellipse:{
            painter->drawEllipse(draw_rect);
            break;
        }
        case Rectangle:
            painter->drawRect(draw_rect);
            break;
        case Line:
            painter->drawLine(draw_rect.x(), draw_rect.y(), draw_rect.x() + draw_rect.width(), draw_rect.y() + draw_rect.height());
            break;
        default:
            break;
        }
    }
    delete painter;
}

void PaintArea::drawPath(QPixmap *pixmap, double scale){

    QPainter *painter = new QPainter(pixmap);
    for(vector<QPoint>::iterator it = movePathVec.begin(); it + 1 < movePathVec.end(); it++){
        pen.setColor(Qt::blue);
        painter->setPen(pen);
        painter->drawLine(*it, *(it+1));
    }

    delete painter;
}

void PaintArea::drawRobot(QPixmap *pixmap, double scale){
    //    if(!robot->getIsRobotAvailable())
    //        return;
    //    QPainter *painter = new QPainter(pixmap);
    //    if(robot){
    //        //        cout << "robot pose" <<robot->getPose().x << " " << robot->getPose().y << endl;
    //        pen.setColor(Qt::black);
    //        painter->setPen(pen);
    //        painter->setBrush(QBrush(Qt::NoBrush));
    //        Pose pose = robot->getPose();
    //        QPointF origin = QPointF(pose.x * scale, pose.y * scale);
    //        double r_in = Robot::radius / PaintArea::resolution * scale;
    //        double r_out = Robot::scan_radius / PaintArea::resolution * scale;
    //        painter->drawEllipse(origin, r_in, r_in);
    //        painter->setPen(QPen(Qt::green, 1, Qt::DashDotLine));
    //        painter->drawEllipse(origin, r_out, r_out);
    //        double start_x = pose.x;
    //        double start_y = pose.y;
    //        double yaw = pose.yaw;
    //        //        double end_x = start_x + r_in * cos(yaw);
    //        //        double end_y = start_y + r_in * sin(yaw);
    //        painter->setPen(QPen(Qt::red, 2));
    //        //        painter->drawLine(QPointF(start_x, start_y), QPointF(end_x, end_y));
    //        painter->drawPoint(start_x, start_y);
    //    }
    //    delete painter;
    Pose pose = robot->thread->getRobotPose(rotateMat);
    if(pose.x < 0) return;
    QPainter *painter = new QPainter(pixmap);
    painter->setPen(QPen(Qt::red, 2));
    painter->drawPoint(pose.x, pose.y);
    delete painter;
}

void PaintArea::drawPixmap(QPixmap *pixmap, double scale){

    //    QPixmap new_pixmap = QPixmap(PaintArea::pixmap_width * scale, PaintArea::pixmap_height * scale);
    QPixmap new_pixmap = origin_pixmap;
    //    QPixmap new_pixmap = *pixmap;
    //    new_pixmap.fill(Qt::white);
    drawMap(&new_pixmap, scale);
    drawPath(&new_pixmap, scale);
    drawRobot(&new_pixmap, scale);

    //    QPixmap show_map((PaintArea::pixmap_width + ext_pixel) * scale, (PaintArea::pixmap_height + ext_pixel) * scale);
    //    painter->drawPixmap(ext_pixel / 2 * scale, ext_pixel / 2 * scale, new_pixmap);
    this->pixmap = new_pixmap;
    update();
}

void PaintArea::loadMap(){
    QString path = QFileDialog::getOpenFileName(this, "", "/home");
    rotateMat = getMat(path);
    origin_pixmap = QPixmap::fromImage(cvMat2QImage(rotateMat.mat));
    pixmap_width = origin_pixmap.width();
    pixmap_height = origin_pixmap.height();
    drawStartPos.setX((size.width() - pixmap_width * scale) / 2);
    drawStartPos.setY((size.height() - pixmap_height * scale) / 2);
    pixmap = origin_pixmap;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(show_path()));
    timer->start(200);
    update();
}

void PaintArea::saveMap(){
    QString path = QFileDialog::getSaveFileName(this, "", "/home");
    QPixmap map;
    for(int i = 0; i< obstacleVec.size(); i++){
        obstacleVec.at(i).isSelected = false;
    }
    drawPixmap(&map, 1);
    map.save(path.toAscii());
    for(int i = 0; i < obstacleSelectedIndexVec.size(); i++){
        obstacleVec.at(obstacleSelectedIndexVec.at(i)).isSelected = true;
    }
}

void PaintArea::show_path(){
    drawPixmap(&origin_pixmap, 1);
    return;
    Pose pose = robot->getPose();
    int x = pose.x;
    int y = pose.y;
    int **localMap;
    int localMap_left = (x - SCAN_PIXELS_RADIUS > 0 ? x - SCAN_PIXELS_RADIUS : 0);
    int localMap_top = (y - SCAN_PIXELS_RADIUS > 0 ? y - SCAN_PIXELS_RADIUS : 0);
    int localMap_right = (x + SCAN_PIXELS_RADIUS < pixmap.width() ? x + SCAN_PIXELS_RADIUS : pixmap.width() - 1);
    int localMap_bottom = (y + SCAN_PIXELS_RADIUS < pixmap.height() ? y + SCAN_PIXELS_RADIUS : pixmap.height() - 1);
    int localMap_width = localMap_right - localMap_left + 1;
    int localMap_height = localMap_bottom - localMap_top + 1;

    //    cout << "width = " << localMap_width << "height = " << localMap_height << endl;

    localMap = new int*[localMap_width];
    for(int i=0;i<localMap_width;i++){
        localMap[i]=new int[localMap_height];
    }

    QPixmap pixmap = QPixmap(PaintArea::pixmap_width, PaintArea::pixmap_height);
    pixmap.fill(Qt::white);
    drawMap(&pixmap, 1);
    //    cout << "init local_map" << endl;
    for(int i = 0; i < localMap_width; i++){
        for(int j = 0; j < localMap_height; j++){
            localMap[i][j] = qGray(((pixmap.toImage()).pixel(localMap_left+i, localMap_top+j)));
        }
    }
    //    cout << "init local_map success" << endl;
    robot->setLocalMap(localMap, localMap_left, localMap_top, localMap_width, localMap_height);
    delete localMap;

    if(robot){
        robot->move(1.0);
    }

    movePathVec.push_back(QPoint(robot->getPose().x, robot->getPose().y));
    drawPixmap(&this->pixmap, 1);
    cout << "path size = " << robot->getPath().size() << " pathIndex = " << robot->getpathIndex() << endl;
    if(robot->getPath().size() > robot->getpathIndex()){
        //        cout << "path size = " << robot->getPath().size() << " pathIndex = " << robot->getpathIndex() << endl;
    }
    else {
        cout << "reached the end" << endl;
        timer->stop();
        delete timer;
        timer = NULL;
    }
}

void PaintArea::build_path(){
    cout << "build path" << endl;

    QPixmap pixmap = QPixmap(PaintArea::pixmap_width * scale, PaintArea::pixmap_height * scale);
    pixmap.fill(Qt::white);
    drawMap(&pixmap, scale);
    QImage img = pixmap.toImage();
    robot->setImg(img);
    path = get_path(img);
    path_index = 0;
    if(robot){
        robot->setPath(path);
        robot->setpathIndex(0);
    }

    movePathVec.clear();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(show_path()));
    timer->start(100);
}

void PaintArea::testAvoidObstacle(){
    movePathVec.clear();
    QPixmap pixmap = origin_pixmap;
    drawMap(&pixmap, 1);
    QImage img = pixmap.toImage();
    robot->setImg(img);
    path = get_path(img);
    path_index = 0;
    robot->setPath(path);
    robot->setpathIndex(0);
    vector<Vertex> vec;
    robot->setvisitedVec(vec);
    robot->setunreachedVertexVec(vec);
    cout << "start move" << endl;
    show_path();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(show_path()));
    timer->start(200);
}

vector<ObstacleScanned> getObstacleVec(QImage img);
vector<Point> getObstacleScanPath(QImage img);

//test pixel color error when exchange pixmap to img
void PaintArea::test(){
    QPixmap pixmap = QPixmap(PaintArea::pixmap_width * scale, PaintArea::pixmap_height * scale);
    pixmap.fill(Qt::white);
    drawMap(&pixmap, scale);
    QImage img = pixmap.toImage();
    vector<ObstacleScanned> obstacleVec = getObstacleVec(img);
    QPixmap new_pixmap = QPixmap(PaintArea::pixmap_width * scale, PaintArea::pixmap_height * scale);
    new_pixmap.fill(Qt::white);
    img = new_pixmap.toImage();
    for(vector<ObstacleScanned>::iterator it = obstacleVec.begin(); it != obstacleVec.end(); it++){
        vector<Point> vec = (*it).pointVec;
        for(int i = 0; i < vec.size(); i++){
            int x = vec.at(i).x;
            int y = vec.at(i).y;
            img.setPixel(x, y, 100);
        }
    }
    this->pixmap = QPixmap::fromImage(img);
    update();
}

void PaintArea::test_2(){
    QPixmap pixmap = QPixmap(PaintArea::pixmap_width * scale, PaintArea::pixmap_height * scale);
    pixmap.fill(Qt::white);
    drawMap(&pixmap, scale);
    QImage img = pixmap.toImage();
    cout << "output color" << endl;
    vector<Vertex> obstacleVec = get_path(img);
    QPixmap new_pixmap = QPixmap(PaintArea::pixmap_width * scale, PaintArea::pixmap_height * scale);
    new_pixmap.fill(Qt::white);
    img = new_pixmap.toImage();
    for(vector<Vertex>::iterator it = obstacleVec.begin(); it != obstacleVec.end(); it++){
        int x = (*it).point.x;
        int y = (*it).point.y;
        img.setPixel(x, y, 100);
    }
    this->pixmap = QPixmap::fromImage(img);
    update();
}

void PaintArea::paintEvent(QPaintEvent *){

    if(pixmap.width() == 0 && pixmap.height() == 0) return;
    QPainter painter(this);
    painter.drawPixmap(QPointF(drawStartPos.x(), drawStartPos.y()), pixmap.scaled(pixmap_width * scale, pixmap_height * scale, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    //  draw arrow
    //            double arrow_lenght_ = 10;
    //            double arrow_degrees_ = 0.5;
    //            double x1 = end_x - arrow_lenght_ * cos(yaw - arrow_degrees_);
    //            double y1 = end_y - arrow_lenght_ * sin(yaw - arrow_degrees_);
    //            double x2 = end_x - arrow_lenght_ * cos(yaw + arrow_degrees_);
    //            double y2 = end_y - arrow_lenght_ * sin(yaw + arrow_degrees_);
    //            p.drawLine(end_x, end_y, x1, y1);
    //            p.drawLine(end_x, end_y, x2, y2);
}
