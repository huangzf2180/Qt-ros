#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>
#include<QPen>
#include<QBrush>
#include<QPainter>
#include<QMouseEvent>
#include<vector>
#include<QMenu>
#include<QTimer>
#include"obstacle.h"
#include"modifydialog.h"
#include"robot.h"
#include"map.h"
#include"common.h"

using std::vector;

class PaintArea : public QWidget
{
    Q_OBJECT
public:
    explicit PaintArea(QWidget *parent = 0);
    void init();
    void setShape(Shape);
    void setPen(QPen);
    void setBrush(QBrush);
    void setFillRule(Qt::FillRule);
    void setIsShapeSelected(bool);
    bool getIsShapeSelected();
    void setIsObstacleSelected(bool);
    bool getIsObstacleSelected();
    int obstacleSelect(vector<int>, QPointF);
    void moveObstacle(float, float);
    void drawPixmap(QPixmap *, double);
    void drawMap(QPixmap *, double);
    void drawPath(QPixmap *, double);
    void drawRobot(QPixmap *, double);
    void saveMap();
    void loadMap();
    void build_path();
    void test();
    void test_2();
    void testAvoidObstacle();

    QPointF moveStartPos, moveEndPos;
    QMenu contextMenu;
    QAction *modifyAction;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void resizeEvent(QResizeEvent *);
    static PaintArea* getInstance();

    static double real_height;
    static double real_width;
    static double pixmap_height;
    static double pixmap_width;
    static double resolution;
    static QPointF drawStartPos;
    static PaintArea *paintArea;

    double scale;

    Object putObject;
    QPixmap origin_pixmap;
    QPixmap pixmap;
    QSize size;




private:
    Shape shape;
    bool isShapeSelected;
    bool isObstacleSelected;
    bool isRobotSelected;
    QPainter *painter;
    QPen pen;
    QBrush brush;
    Qt::FillRule fillRule;
    QPointF startPos, endPos;
    vector<Obstacle> obstacleVec;
    Robot *robot;
    vector<int> obstacleSelectedIndexVec;
    int obstacleSelectedIndex;
    ModifyDialog *modifyDialog;
    vector<Vertex> path;
    vector<QPoint> movePathVec;
    int path_index;
    QTimer *timer;
    Map* map;
    RotateMat rotateMat;



    int isPointInPolygon(QPointF);
    void obstacleSelected(vector<Obstacle>, vector<int>);
    bool isMouseInDrawArea(QPointF);

signals:

public slots:
    void showModifyDialog();
    void show_path();

};

#endif // PAINTAREA_H
