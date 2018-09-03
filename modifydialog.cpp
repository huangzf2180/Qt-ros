#include "modifydialog.h"
#include<iostream>
#include"paintarea.h"

using std::cout;
using std::endl;

ModifyDialog::ModifyDialog(vector<Obstacle> &obstacleVec, vector<int> &obstacleSelectedIndexVec, QWidget *parent) :
    QDialog(parent), obstacleVec(obstacleVec), obstacleSelectedIndexVec(obstacleSelectedIndexVec)
{

    setWindowTitle(tr("properties modified"));
    posxLabel = new QLabel();
    posxLabel->setText(tr("X(m):"));
    posyLabel = new QLabel();
    posyLabel->setText(tr("Y(m):"));
    widthLable = new QLabel();
    widthLable->setText(tr("Width(m):"));
    heightLable = new QLabel();
    heightLable->setText(tr("Height(m):"));

    posxEdit = new QLineEdit();
    posyEdit = new QLineEdit();
    widthEdit = new QLineEdit();
    heightEdit = new QLineEdit();

    buttonBox = new QDialogButtonBox();
    buttonBox->addButton("confirm", QDialogButtonBox::YesRole);
    buttonBox->addButton("cancle", QDialogButtonBox::NoRole);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(confirm()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));


    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(posxLabel, 0, 0);
    mainLayout->addWidget(posxEdit, 0, 1);
    mainLayout->addWidget(posyLabel, 0, 2);
    mainLayout->addWidget(posyEdit, 0, 3, Qt::AlignLeft);
    mainLayout->addWidget(widthLable, 1, 0);
    mainLayout->addWidget(widthEdit, 1, 1);
    mainLayout->addWidget(heightLable, 1, 2);
    mainLayout->addWidget(heightEdit, 1, 3);
    mainLayout->addWidget(buttonBox, 2, 0, 1, 4, Qt::AlignCenter);
    mainLayout->setVerticalSpacing(10);

    for(vector<int>::iterator it = obstacleSelectedIndexVec.begin(); it != obstacleSelectedIndexVec.end(); it++){
        QRectF rect = obstacleVec.at(*it).getRect();
        posxEdit->setText(QString::number(rect.x() * PaintArea::resolution));
        posyEdit->setText(QString::number(rect.y() * PaintArea::resolution));
        widthEdit->setText(QString::number(rect.width() * PaintArea::resolution));
        heightEdit->setText(QString::number(rect.height() * PaintArea::resolution));
    }

}

ModifyDialog::~ModifyDialog(){
//    delete posxLabel;
//    delete posyLabel;
//    delete posxEdit;
//    delete posyEdit;
//    delete positionBtn;
//    delete width
}

void ModifyDialog::setObstacleVec(vector<Obstacle> &obstacleVec){
    this->obstacleVec = obstacleVec;
}

void ModifyDialog::setObstacleSelectedIndexVec(vector<int> &obstacleSelectedIndexVec){
    this->obstacleSelectedIndexVec = obstacleSelectedIndexVec;
}

void ModifyDialog::confirm(){
    double x = posxEdit->text().toDouble();
    double y = posyEdit->text().toDouble();
    double width = widthEdit->text().toDouble();
    double height = heightEdit->text().toDouble();
    for(vector<int>::iterator it = obstacleSelectedIndexVec.begin(); it != obstacleSelectedIndexVec.end(); it++){
        obstacleVec.at(*it).setRect(QRectF(x / PaintArea::resolution, y / PaintArea::resolution, width / PaintArea::resolution, height / PaintArea::resolution));
    }

    accept();
}

void ModifyDialog::cancel(){
    reject();
}
