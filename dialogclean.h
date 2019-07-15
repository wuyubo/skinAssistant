#ifndef DIALOGCLEAN_H
#define DIALOGCLEAN_H
#include "interface.h"
#include <QDialog>

namespace Ui {
class DialogClean;
}

class DialogClean : public QDialog
{
    Q_OBJECT

public:
    explicit DialogClean(InterFace *_pinterface, QWidget *parent = 0);
    ~DialogClean();
    bool isCheckImgeUse(QString imgID);
    bool isCheckStrUse(QString strID);

private slots:
    void on_ptn_relink_clicked();

    void on_ptn_string_clicked();

    void on_ptn_image_clicked();

    void on_ptn_changeFont_clicked();

private:
    Ui::DialogClean *ui;
    InterFace *pinterface;
};

#endif // DIALOGCLEAN_H
