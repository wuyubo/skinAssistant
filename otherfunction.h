#ifndef OTHERFUNCTION_H
#define OTHERFUNCTION_H

#include <QDialog>

namespace Ui {
class OtherFunction;
}

class OtherFunction : public QDialog
{
    Q_OBJECT

public:
    explicit OtherFunction(QWidget *parent = 0);
    ~OtherFunction();
    void MApp_E_OSD_RestTransition(int w, int h, int x, int y);

private slots:
    void on_ptn_define_clicked();

private:
    Ui::OtherFunction *ui;
    QString output;
    QStringList defineList;
};

#endif // OTHERFUNCTION_H
