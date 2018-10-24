#include "dialogui.h"
#include "ui_dialogui.h"

DialogUI::DialogUI(InterFace *_pinterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUI)
{
    ui->setupUi(this);
    pinterface = _pinterface;
}

DialogUI::~DialogUI()
{
    delete ui;
}

void DialogUI::showWndToUi(Menu_Wnd *wnd)
{
    QDomNode n = wnd->node.firstChild();
    bool ok;

    if(wnd->label == NULL)
    {
        wnd->label = new XLabel(ui->groupBox);
    }
    //wnd->label->setParent(ui->groupBox);
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position")
           {
                wnd->label->setGeometry(e.toElement().attribute("X").toInt(&ok),
                                        e.toElement().attribute("Y").toInt(&ok),
                                        e.toElement().attribute("Width").toInt(&ok),
                                        e.toElement().attribute("Height").toInt(&ok));
           }
           if(e.tagName() == "Text")
           {
                if(e.toElement().attribute("TextID")!= "")
                {
                    QString color = e.toElement().attribute("NormalTextColor");
                    color.replace("#", "");
                    QPalette pa;
                    pa.setColor(QPalette::WindowText,QColor(QRgb(color.toInt(&ok,16))));
                    wnd->label->setPalette(pa);

                    QString align = e.toElement().attribute("TextAlign");
                    if(align == "0")
                    {
                        wnd->label->setAlignment(Qt::AlignLeft);
                    }
                    else if(align == "3")
                    {
                        wnd->label->setAlignment(Qt::AlignCenter);
                    }
                    else if(align == "2")
                    {
                        wnd->label->setAlignment(Qt::AlignRight);
                    }
                    wnd->label->setStyleSheet("font: 75 12pt '微软雅黑';");
                    wnd->label->setText(pinterface->getString(e.toElement().attribute("TextID"), "English"));
                }

           }
           if(e.tagName () == "StaticWndProperties")
           {
                QString state = e.toElement().attribute("BgState");

                if(state == "0")
                {
                    QString clone = e.toElement().attribute("Clone");
                    Menu_Wnd *w = pinterface->getWndFromId(clone);
                    if(w != NULL)
                    {
                        showCloneStaticWndProperties(wnd, w);
                    }

                }/*else if(state == "1")
                {
                    wnd->label->setPixmap(QPixmap(""));
                    if(e.toElement().attribute("HasNormalDrawStyle") == "1")
                    {
                        QString color = e.toElement().attribute("NormalBgColor");
                        color = "background-color:"+color;
                        wnd->label->setStyleSheet(color);
                    }else
                    {
                        wnd->label->setStyleSheet("");
                    }
                }*/
                else if(state == "2")
                {
                    showIconFromId(e.toElement().attribute("NormalBitmapID"), wnd->label);

                }


           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
    wnd->label->show();
    wnd->isShow = true;
}

void DialogUI::hideWndFromUi(Menu_Wnd *wnd)
{
     if(wnd->label != NULL)
     {
         wnd->label->hide();
         wnd->isShow = false;
     }
}

void DialogUI::refreshWndFromUi(Menu_Wnd *wnd)
{

    if(Qt::Checked == wnd->item->checkState(0))
    {
        showWndToUi(wnd);
    }
    else
    {
        hideWndFromUi(wnd);
    }
}


void DialogUI::showTreetoUI(Menu_Wnd *w)
{
    qDebug()<<""<<w->frame;
    refreshWndFromUi(w);

    while(w != w->rbroher)
    {
        w = w->rbroher;
        refreshWndFromUi(w);
        if((w->firstChild != w))
        {
            if((w->isShow))
            {
                showTreetoUI(w->firstChild);
            }else
            {
                hideTreeFromUI(w->firstChild);
            }
        }

    }
}
void DialogUI::hideTreeFromUI(Menu_Wnd *w)
{
    hideWndFromUi(w);
    while(w != w->rbroher)
    {
        w = w->rbroher;
        hideWndFromUi(w);
        if(w->firstChild != w)
        {
            hideTreeFromUI(w->firstChild);
        }
    }
}

void DialogUI::refreshUI()
{
    foreach(Menu_Wnd * wnd, pinterface->menuWndList)
    {
        if(wnd->node.toElement().attribute("CtrlTypeName") == "Main Frame")
        {
            if(wnd->frame == pinterface->getCurframe())
            {
                showTreetoUI(wnd);
            }else
            {
               hideTreeFromUI(wnd);

            }
        }
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
                    wnd->label->setPixmap(QPixmap(""));
                    if(e.toElement().attribute("HasNormalDrawStyle") == "1")
                    {
                        QString color = e.toElement().attribute("NormalBgColor");
                        color = "background-color:"+color;
                        show_wnd->label->setStyleSheet(color);
                    }else
                    {
                        show_wnd->label->setStyleSheet("");
                    }

                }
                else if(state == "2")
                {
                    showIconFromId(e.toElement().attribute("NormalBitmapID"), show_wnd->label);

                }
            }
        }
        n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
     }
}

void DialogUI::showIconFromId(QString ID, XLabel *lb_icon)
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

void DialogUI::showIcon(XImg *img, XLabel *lb_icon)
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
    qDebug() << filepath;
    QPixmap src(filepath);//QPixmap src;   src.load(fileName);
   // ui->label->setPixmap(src.scaled(lb_icon->size()));
    lb_icon->setPixmap(src.scaled(lb_icon->size()));//若改变图像适应label，则 ui->label->setPixmap(src.scaled(ui->label->size()));
}

