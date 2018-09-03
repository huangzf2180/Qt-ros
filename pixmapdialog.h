#ifndef PIXMAPDIALOG_H
#define PIXMAPDIALOG_H

#include <QDialog>
#include<QLabel>
#include<QLineEdit>
#include<QGridLayout>
#include<QDialogButtonBox>
#include"paintarea.h"

class PixmapDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PixmapDialog(QWidget *parent = 0);

private:
    QLabel *widthLabel;
    QLabel *heightLabel;
    QLabel *resolutionLabel;

    QLineEdit *widthEdit;
    QLineEdit *heightEdit;
    QLineEdit *resolutionEdit;

    QDialogButtonBox *buttonBox;

    QGridLayout *mainLayout;

signals:

public slots:
    void confirm();
    void cancel();

};

#endif // PIXMAPDIALOG_H
