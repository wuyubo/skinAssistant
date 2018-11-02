#ifndef DIALOGUI_H
#define DIALOGUI_H
#include "global.h"
#include "setting.h"
#include "interface.h"
#include "dialogui.h"
#include "xlabel.h"
#include <QDebug>
#include <QMainWindow>
#include <QFile>
#include <QXmlStreamReader>
#include <QTreeWidgetItem>
#include "zxml.h"
#include <QStandardItem>
#include <QLabel>
#include <QDialog>

namespace Ui {

#define  UI_WIDTH  1366
#define  UI_HEIGHT 768
class DialogUI;
}

class DialogUI : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUI(InterFace *_pinterface, QWidget *parent = 0);
    ~DialogUI();
    void showWndToUi(Menu_Wnd *wnd);
    void hideWndFromUi(Menu_Wnd *wnd);
    void refreshWndFromUi(Menu_Wnd *wnd);
    void showTreetoUI(Menu_Wnd *wnd);
    void hideTreeFromUI(Menu_Wnd *w);
    void showCloneStaticWndProperties(Menu_Wnd *show_wnd, Menu_Wnd *wnd);
    void showIconFromId(QString ID, QLabel *lb_icon);
    void showIcon(XImg *img, QLabel *lb_icon);
    int getUIx(int _x);
    int getUIy(int _y);
    int getUIh(int _h);
    int getUIw(int _w);
public slots:
    void refreshUI();

private:
    Ui::DialogUI *ui;
    InterFace *pinterface;
    float map_width;
    float map_height;
};

#endif // DIALOGUI_H
