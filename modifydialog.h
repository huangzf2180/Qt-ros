#ifndef MODIFYDIALOG_H
#define MODIFYDIALOG_H

#include <QDialog>
#include<QLabel>
#include<QLineEdit>
#include<QPushButton>
#include<vector>
#include<QGridLayout>
#include<QDialogButtonBox>
#include"obstacle.h"

using std::vector;


class ModifyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ModifyDialog(vector<Obstacle> &, vector<int> &, QWidget *parent = 0);
    ~ModifyDialog();
    void setObstacleVec(vector<Obstacle> &);
    void setObstacleSelectedIndexVec(vector<int> &);

private:
    vector<Obstacle> &obstacleVec;
    vector<int> &obstacleSelectedIndexVec;

    QLabel *posxLabel;
    QLabel *posyLabel;
    QLabel *widthLable;
    QLabel *heightLable;

    QLineEdit *posxEdit;
    QLineEdit *posyEdit;
    QLineEdit *widthEdit;
    QLineEdit *heightEdit;

    QPushButton *positionBtn;
    QPushButton *sizeBtn;

    QDialogButtonBox *buttonBox;

    QGridLayout *mainLayout;


signals:

public slots:
    void confirm();
    void cancel();
};

#endif // MODIFYDIALOG_H
