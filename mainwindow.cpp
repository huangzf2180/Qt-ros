#include "mainwindow.h"
#include<iostream>
#include<QIcon>

using std::cout;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    int argc = 0;
    char *argv[] = {};
    ros::init(argc, argv, "qt_camera");
    setWindowTitle(tr("QT"));
    paintArea = new PaintArea(this);
    setCentralWidget(paintArea);
    createActions();
    creatMenus();
    createToolBars();

    pixmapDialog = NULL;

    //    QHBoxLayout *mainLayout =new QHBoxLayout(this);
    //    mainLayout->addWidget(paintArea);
    //    addDockWidget(paintArea);
}

MainWindow::~MainWindow()
{

}

void MainWindow::creatMenus(){
//    fileMenu = menuBar()->addMenu(tr("File"));
//    fileMenu->addAction(newFileAction);
}

void MainWindow::createActions(){
    //File Action
    newFileAction = new QAction(tr("new"), this);
    newFileAction->setShortcut(tr("Ctrl_N"));
    connect(newFileAction, SIGNAL(triggered(bool)), this, SLOT(showPixmapDialog()));
    newFileAction->setStatusTip(tr("open a new file"));
    openFileAction = new QAction(tr("open"), this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    connect(openFileAction, SIGNAL(triggered(bool)), this, SLOT(loadMap()));
    saveFileAction = new QAction(tr("save"), this);
    saveFileAction->setShortcut(tr("Ctrl+S"));
    connect(saveFileAction, SIGNAL(triggered(bool)), this, SLOT(saveMap()));
    //Obstacle Action
    putEllipseAction = new QAction(tr("ellipse"), this);
    connect(putEllipseAction, SIGNAL(triggered()), this, SLOT(setShapeEllipse()));
    putRectangleAction = new QAction(tr("rectangle"), this);
    connect(putRectangleAction, SIGNAL(triggered()), this, SLOT(setShapeRectangle()));
    putLineAction = new QAction(tr("line"), this);
    connect(putLineAction, SIGNAL(triggered()), this, SLOT(setShapeLine()));
    putRobotAction = new QAction(tr("robot"), this);
    connect(putRobotAction, SIGNAL(triggered()), this, SLOT(setRobot()));
    inspectingAction = new QAction(tr("inspecting"), this);
    connect(inspectingAction, SIGNAL(triggered(bool)), this, SLOT(inspecting()));
    //Path Action
    buildPathAction = new QAction(tr("build path"), this);
    connect(buildPathAction, SIGNAL(triggered(bool)), this, SLOT(buildPath()));
    //Camera Action
    readFrameAction = new QAction(tr("camera"), this);
    connect(readFrameAction, SIGNAL(triggered(bool)), this, SLOT(displayFrame()));
}

void MainWindow::createToolBars(){
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fileTool = addToolBar(tr("File"));
    fileTool->addAction(newFileAction);
    fileTool->addAction(openFileAction);
    fileTool->addAction(saveFileAction);
    obstacleTool = addToolBar(tr("Obstacle"));
    obstacleTool->addAction(putEllipseAction);
    obstacleTool->addAction(putRectangleAction);
    obstacleTool->addAction(putLineAction);
    robotTool = addToolBar(tr("Robot"));
    robotTool->addAction(putRobotAction);
    robotTool->addAction(inspectingAction);
    pathTool = addToolBar(tr("path"));
    pathTool->addAction(buildPathAction);
    //Camera Tool
    cameraTool = addToolBar(tr("Camera"));
    cameraTool->addAction(readFrameAction);
}

//void MainWindow::setObstacle(Shape s){
//    paintArea->putObject = Object_Obstacle;
//    paintArea->setShape(s);
//    paintArea->setIsShapeSelected(true);
//}

void MainWindow::setRobot(){
    paintArea->putObject = Object_Robot;
}

void MainWindow::setShapeEllipse(){
    paintArea->putObject = Object_Obstacle;
    paintArea->setShape(Ellipse);
    paintArea->setIsShapeSelected(true);
}

void MainWindow::setShapeRectangle(){
    paintArea->putObject = Object_Obstacle;
    paintArea->setShape(Rectangle);
    paintArea->setIsShapeSelected(true);
}

void MainWindow::setShapeLine(){
    paintArea->putObject = Object_Obstacle;
    paintArea->setShape(Line);
    paintArea->setIsShapeSelected(true);
}

void MainWindow::ShowShape(int value){
    paintArea->setShape(Ellipse);
}

void MainWindow::showPixmapDialog(){
    pixmapDialog = new PixmapDialog(this);
    int res = pixmapDialog->exec();
    if(res == QDialog::Accepted){
        QSize size = paintArea->geometry().size();
        PaintArea::drawStartPos.setX((size.width() - PaintArea::pixmap_width) / 2);
        PaintArea::drawStartPos.setY((size.height() - PaintArea::pixmap_height) / 2);
        paintArea->scale = 1.0;
        paintArea->drawPixmap(&(paintArea->pixmap), 1.0);
    }
    delete pixmapDialog;
}

void MainWindow::saveMap(){
    paintArea->saveMap();
}

void MainWindow::loadMap(){
    paintArea->loadMap();
}

void MainWindow::buildPath(){
    paintArea->testAvoidObstacle();
}

void MainWindow::inspecting(){
    int a = 0;
    ros::init(a, NULL, "aaa");
    ros::NodeHandle nh;
    ros::Duration(10).sleep();
//    Robot* robot = Robot::getInstance();
//    robot->inspecting();
}

void MainWindow::displayFrame(){
    cameraDialog = new Camera(this);
    cameraDialog->exec();
}
