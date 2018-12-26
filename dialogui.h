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


class DialogUI;
}

#define  UI_WIDTH  1366
#define  UI_HEIGHT 768

typedef enum
{
    FONT_BIG,
    FONT_NORMAL,
    FONT_SMALL,
    FONT_MAX,
}FONT_SIZE;

typedef enum
{
    BG_CLONE,
    BG_COLOR,
    BG_IAMGE,
    BG_MAX,
}BG_TYPE;

class DialogUI : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUI(InterFace *_pinterface, QWidget *parent = 0);
    ~DialogUI();
    void showWndToUi(Menu_Wnd *wnd);
    void hideWndFromUi(Menu_Wnd *wnd);
    void showCloneStaticWndProperties(Menu_Wnd *show_wnd, Menu_Wnd *wnd);
    void showIconFromId(QString ID, QLabel *lb_icon);
    void showIcon(XImg *img, QLabel *lb_icon);
    int getUIx(int _x);
    int getUIy(int _y);
    int getUIh(int _h);
    int getUIw(int _w);
    void setWPostion(Menu_Wnd *wnd, int _x, int _y, int _w, int _h);
    void setWText(Menu_Wnd *wnd, QString text, QPalette *pcolor = NULL, ALIGN_MODE align = ALIGN_CENTER, FONT_SIZE size = FONT_NORMAL);
    void setWBackground(Menu_Wnd *wnd, BG_TYPE type, QString color = NULL, QString image = NULL, Menu_Wnd *cloneWnd = NULL);
    bool checkIsWndShow(Menu_Wnd *wnd);
public slots:
    void refreshUI();

private slots:


    void on_ptn_refresh_clicked();

private:
    Ui::DialogUI *ui;
    InterFace *pinterface;
    float map_width;
    float map_height;
};

#endif // DIALOGUI_H
