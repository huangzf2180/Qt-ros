#include "pixmapdialog.h"
#include<QDialogButtonBox>
#include<iostream>


using std::cout;
using std::endl;

PixmapDialog::PixmapDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("set pixmap size"));

    widthLabel = new QLabel();
    widthLabel->setText(tr("width(m):"));
    heightLabel = new QLabel();
    heightLabel->setText(tr("height(m):"));
    resolutionLabel = new QLabel();
    resolutionLabel->setText(tr("resolution"));

    widthEdit = new QLineEdit();
    heightEdit = new QLineEdit();
    resolutionEdit = new QLineEdit();

    buttonBox = new QDialogButtonBox();
    buttonBox->addButton("confirm", QDialogButtonBox::YesRole);
    buttonBox->addButton("cancle", QDialogButtonBox::NoRole);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(confirm()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(widthLabel, 0, 0);
    mainLayout->addWidget(widthEdit, 0, 1);
    mainLayout->addWidget(heightLabel, 1, 0);
    mainLayout->addWidget(heightEdit, 1, 1);
    mainLayout->addWidget(resolutionLabel, 2, 0);
    mainLayout->addWidget(resolutionEdit, 2, 1);
    mainLayout->addWidget(buttonBox, 3, 1);

}

void PixmapDialog::confirm(){
    double width = widthEdit->text().toDouble();
    double height = heightEdit->text().toDouble();
    double resolution = resolutionEdit->text().toDouble();
    PaintArea::real_width = width;
    PaintArea::real_height = height;
    PaintArea::resolution = resolution;
    PaintArea::pixmap_width = width / resolution;
    PaintArea::pixmap_height = height / resolution;
    accept();
}

void PixmapDialog::cancel(){
    reject();
}
