#include "dialogui.h"
#include "ui_dialogui.h"
#define UIDEBUG(x) //DEBUG(x)
DialogUI::DialogUI(InterFace *_pinterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUI)
{
    ui->setupUi(this);
    pinterface = _pinterface;
    ui->rbn_normal->setChecked(true);
}

DialogUI::~DialogUI()
{
    delete ui;
}

int DialogUI::getUIx(int _x)
{
    return (int)_x*map_width;
}
int DialogUI::getUIy(int _y)
{
    return (int)_y*map_height;
}

int DialogUI::getUIh(int _h)
{
    return (int)_h*map_height;
}
int DialogUI::getUIw(int _w)
{
     return (int)_w*map_width;
}
///////////////////////////////////////////////
/// \brief DialogUI::showWndToUi
/// \param wnd
//////////////////////////////////////
///
void DialogUI::setWPostion(Menu_Wnd *wnd, int _x, int _y, int _w, int _h)
{
    if(wnd->label == NULL)
    {
        return;
    }
    wnd->label->setGeometry(_x, _y, _w, _h);
}
void DialogUI::setWText(Menu_Wnd *wnd, QString text, QPalette *pcolor, ALIGN_MODE align, FONT_SIZE size )
{
    if(wnd->label == NULL)
    {
        return;
    }
    wnd->label->setText(text);
    wnd->label->setPalette(*pcolor);
    switch (align) {
    case ALIGN_LEFT:
        wnd->label->setAlignment(Qt::AlignLeft);
        break;
    case ALIGN_RIGHT:
        wnd->label->setAlignment(Qt::AlignRight);
        break;
    case ALIGN_CENTER:
        wnd->label->setAlignment(Qt::AlignCenter);
        break;
    default:
        break;
    }

    switch (size) {
    case FONT_BIG:
        wnd->label->setStyleSheet("font: 100 12pt '微软雅黑';");
        break;
    case FONT_NORMAL:
        wnd->label->setStyleSheet("font: 75 12pt '微软雅黑';");
        break;
    case FONT_SMALL:
        wnd->label->setStyleSheet("font: 45 12pt '微软雅黑';");
        break;
    default:
        break;
    }

    // qDebug()<< text;

}

void DialogUI::setWBackground(Menu_Wnd *wnd, BG_TYPE type, QString color, QString image, Menu_Wnd *cloneWnd)
{
    if(wnd->label == NULL)
    {
        return;
    }
    switch (type) {
    case BG_CLONE:
        if(cloneWnd != NULL)
        {
            showCloneStaticWndProperties(wnd, cloneWnd);
        }
        break;
    case BG_COLOR:
        wnd->label->setStyleSheet(color);
        break;
    case BG_IAMGE:
        showIconFromId(image, wnd->label);
        break;
    default:
        break;
    }

}

//////////////////////////////////////////////////
void DialogUI::showWndToUi(Menu_Wnd *wnd)
{
    QDomNode n = wnd->node.firstChild();
    bool isText = false;
    bool ok;
    if(wnd->label == NULL)
    {
        wnd->label = new QLabel(ui->groupBox);
    }
    UIDEBUG(wnd->node.toElement().attribute("Name"));


    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position")
           {
                setWPostion(wnd,getUIx(e.toElement().attribute("X").toInt(&ok)),
                            getUIy(e.toElement().attribute("Y").toInt(&ok)),
                            getUIw(e.toElement().attribute("Width").toInt(&ok)),
                            getUIh(e.toElement().attribute("Height").toInt(&ok)));

           }
#if 1
           if(e.tagName() == "Text")
           {
                if(e.toElement().attribute("TextID")!= "")
                {
                    QString color = e.toElement().attribute("NormalTextColor");
                    QString text = pinterface->getString(e.toElement().attribute("TextID"), "English");
                    ALIGN_MODE tAlign = ALIGN_NORMAL;

                    if(ui->rbn_normal->isChecked())
                    {
                        color =e.toElement().attribute("NormalTextColor");
                    }else if(ui->rbn_focus->isChecked())
                    {
                        color = e.toElement().attribute("FocusTextColor");
                    }else if(ui->rbn_disable->isChecked())
                    {
                        color = e.toElement().attribute("DisableTextColor");
                    }

                    color.replace("#", "");
                    QPalette pa;
                    pa.setColor(QPalette::WindowText,QColor(QRgb(color.toInt(&ok,16))));



                    QString align = e.toElement().attribute("TextAlign");
                    if(align == "0")
                    {
                        tAlign = ALIGN_LEFT;
                    }
                    else if(align == "3")
                    {
                        tAlign = ALIGN_CENTER;
                    }
                    else if(align == "2")
                    {
                        tAlign = ALIGN_RIGHT;
                    }

                    setWText(wnd, text, &pa, tAlign);

                    isText = true;
                }

           }
           if(e.tagName () == "StaticWndProperties" && !isText)
           {
                QString state = e.toElement().attribute("BgState");
                //wnd->label->setPixmap(QPixmap(""));
                if(state == "0")
                {
                    QString clone = e.toElement().attribute("Clone");
                    if(clone != "")
                    {
                        Menu_Wnd *w = pinterface->getWndFromId(clone);
                        setWBackground(wnd, BG_CLONE, NULL, NULL, w);

                    }

                }else if(state == "1")
                {
                    QString color = "";

                    if(ui->rbn_normal->isChecked())
                    {
                        if(e.toElement().attribute("HasNormalDrawStyle") == "1")
                        {
                            color = e.toElement().attribute("NormalBgColor");
                            color = "background-color:"+color;
                            setWBackground(wnd, BG_COLOR, color);
                        }
                    }else if(ui->rbn_focus->isChecked())
                    {
                        if(e.toElement().attribute("HasFocusDrawStyle") == "1")
                        {
                            color = e.toElement().attribute("FocusBgColor");
                            color = "background-color:"+color;
                            setWBackground(wnd, BG_COLOR, color);
                        }
                    }else if(ui->rbn_disable->isChecked())
                    {
                        if(e.toElement().attribute("HasDisabledDrawStyle") == "1")
                        {
                            color = e.toElement().attribute("DisabledBgColor");
                            color = "background-color:"+color;
                            setWBackground(wnd, BG_COLOR, color);
                        }
                    }
                }
                else if(state == "2")
                {
                    QString img = "";
                    if(ui->rbn_normal->isChecked())
                    {
                        img = e.toElement().attribute("NormalBitmapID");
                    }else if(ui->rbn_focus->isChecked())
                    {
                        img = e.toElement().attribute("FocusBitmapID");
                    }else if(ui->rbn_disable->isChecked())
                    {
                       img = e.toElement().attribute("DisabledBitmapID");
                    }

                    if(img != "")
                    {
                        setWBackground(wnd, BG_IAMGE, NULL, e.toElement().attribute("NormalBitmapID"));
                    }
                }
           }
#endif
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
       isText = false;

    }

    wnd->label->raise();
    wnd->label->setWindowFlags(Qt::WindowStaysOnTopHint);
    wnd->label->show();
}

void DialogUI::hideWndFromUi(Menu_Wnd *wnd)
{
     wnd->isShow = false;
     if(wnd->label != NULL)
     {
         wnd->label->hide();
     }
}




void DialogUI::showCloneStaticWndProperties(Menu_Wnd *show_wnd, Menu_Wnd *wnd)
{
     QDomNode n = wnd->node.firstChild();
     while (!n.isNull ())
     {
        if (n.isElement ())
        {
            QDomElement e = n.toElement ();
            if(e.tagName () == "StaticWndProperties")
            {
                QString state = e.toElement().attribute("BgState");

                if(state == "0")
                {

                }else if(state == "1")
                {
                    QString color = "";

                    if(ui->rbn_normal->isChecked())
                    {
                        if(e.toElement().attribute("HasNormalDrawStyle") == "1")
                        {
                            color = e.toElement().attribute("NormalBgColor");
                            color = "background-color:"+color;
                            setWBackground(show_wnd, BG_COLOR, color);
                        }
                    }else if(ui->rbn_focus->isChecked())
                    {
                        if(e.toElement().attribute("HasFocusDrawStyle") == "1")
                        {
                            color = e.toElement().attribute("FocusBgColor");
                            color = "background-color:"+color;
                            setWBackground(show_wnd, BG_COLOR, color);
                        }
                    }else if(ui->rbn_disable->isChecked())
                    {
                        if(e.toElement().attribute("HasDisabledDrawStyle") == "1")
                        {
                            color = e.toElement().attribute("DisabledBgColor");
                            color = "background-color:"+color;
                            setWBackground(show_wnd, BG_COLOR, color);
                        }
                    }
                }
                else if(state == "2")
                {
                    QString img = "";
                    if(ui->rbn_normal->isChecked())
                    {
                        img = e.toElement().attribute("NormalBitmapID");
                    }else if(ui->rbn_focus->isChecked())
                    {
                        img = e.toElement().attribute("FocusBitmapID");
                    }else if(ui->rbn_disable->isChecked())
                    {
                       img = e.toElement().attribute("DisabledBitmapID");
                    }

                    if(img != "")
                    {
                        setWBackground(show_wnd, BG_IAMGE, NULL, e.toElement().attribute("NormalBitmapID"));
                    }
                }
            }
        }
        n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
     }
}

void DialogUI::showIconFromId(QString ID, QLabel *lb_icon)
{
    //qDebug() << ID;
    foreach(XImg * img, pinterface->ImgList)
    {
        if(img->ID == ID)
        {
            showIcon(img, lb_icon);
        }
    }
}

void DialogUI::showIcon(XImg *img, QLabel *lb_icon)
{
    QString filepath;
    QString folder = img->FolderName.replace("\\", "/");
    QString temp = folder.left(6);

    if(temp == "../../")
    {
        filepath = pinterface->pSetting->getRootPath()+folder+"/"+img->FileName;
    }else
    {
        filepath = pinterface->pSetting->getImgPath()+img->FolderName.replace("\\", "/")+"/"+img->FileName;
    }
    //qDebug() << filepath;
    QPixmap src(filepath);//QPixmap src;   src.load(fileName);
    lb_icon->setPixmap(src.scaled(lb_icon->size()));//若改变图像适应label，则 ui->label->setPixmap(src.scaled(ui->label->size()));
}


bool DialogUI::checkIsWndShow(Menu_Wnd *wnd)
{
    if(wnd->item->checkState(0) == Qt::Checked)
    {
        wnd->isShow = true;
        return true;
    }
    return false;
}

void DialogUI::refreshUI()
{
    map_width = (float)UI_WIDTH/(float)pinterface->getWidth();
    map_height = (float)UI_HEIGHT/(float)pinterface->getHeight();
    foreach(Menu_Wnd * wnd, pinterface->menuWndList)
    {
        if(wnd->frame != pinterface->getCurframe())
        {
            wnd->isShow = false;
        }else
        {
            checkIsWndShow(wnd);
        }

        if(wnd->parent->isShow && wnd->isShow)
        {
            showWndToUi(wnd);
        }else
        {
            hideWndFromUi(wnd);
        }
    }

}



void DialogUI::on_ptn_refresh_clicked()
{
    refreshUI();
}
