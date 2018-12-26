#ifndef DIALOGIMAGECLONE_H
#define DIALOGIMAGECLONE_H

#include <QDialog>

#include "global.h"
#include "setting.h"
#include "interface.h"

namespace Ui {
class DialogImageClone;
}

class DialogImageClone : public QDialog
{
    Q_OBJECT

public:
    explicit DialogImageClone(InterFace *_pinterface, QWidget *parent = 0);
    ~DialogImageClone();
    bool CloneImage(XImg *img);
    bool copyImage(QString sourceImg ,QString toImg);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_ptn_addMirrorImg_clicked();

private:
    Ui::DialogImageClone *ui;

    InterFace *pinterface;
};

#endif // DIALOGIMAGECLONE_H
