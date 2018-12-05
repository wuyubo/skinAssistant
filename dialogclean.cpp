#include "dialogclean.h"
#include "ui_dialogclean.h"

DialogClean::DialogClean(InterFace *_pinterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogClean)
{
    ui->setupUi(this);
    pinterface = _pinterface;
}

DialogClean::~DialogClean()
{
    delete ui;
}

void DialogClean::on_ptn_relink_clicked()
{
    QString imgID;
    QString imgLowerID;
    QString temp;
    int count = 0, err_count = 0;
    QString err_img;
    foreach(Menu_Wnd * wnd, pinterface->menuWndList)
    {
        if(wnd != NULL)
        {
            imgID = pinterface->getWinIamgeId(wnd, ST_NOMAL);

            if((imgID != "") && pinterface->checkImgFromID(imgID)==NULL)
            {
                imgLowerID = pinterface->checkImgFromID(imgID, 1);
                if(imgLowerID != NULL)
                {
                    pinterface->setWinIamgeId(wnd, imgLowerID, ST_NOMAL);
                    count++;
                }else
                {
                   qDebug()<<imgID;
                   err_count ++;
                   err_img.append(imgID).append("\r\n");
                }
            }
            imgID = pinterface->getWinIamgeId(wnd, ST_FOCUS);
            if((imgID != "")&&pinterface->checkImgFromID(imgID)==NULL)
            {
                imgLowerID = pinterface->checkImgFromID(imgID, 1);
                if(imgLowerID != NULL)
                {
                    pinterface->setWinIamgeId(wnd, imgLowerID, ST_NOMAL);
                    count++;
                }else
                {
                   qDebug()<<imgID;
                   err_count ++;
                   err_img.append(imgID).append("\r\n");
                }
            }
            imgID = pinterface->getWinIamgeId(wnd, ST_DISABLE);
            if((imgID != "") && pinterface->checkImgFromID(imgID)==NULL)
            {
                imgLowerID = pinterface->checkImgFromID(imgID, 1);
                if(imgLowerID != NULL)
                {
                    pinterface->setWinIamgeId(wnd, imgLowerID, ST_NOMAL);
                    count++;
                }else
                {
                   qDebug()<<imgID;
                   err_count ++;
                   err_img.append(imgID).append("\r\n");
                }
            }
        }
    }
    temp.sprintf("成功重定位图片 %d 个（含重复), \r\n重定位失败 %d 个（含重复img_err.txt）", count, err_count);
    ui->lb_tip->setText(temp);
    pinterface->pSetting->saveFile("./img_err.txt", err_img, true);
}

void DialogClean::on_ptn_string_clicked()
{
    int count = 0;
    QString temp;
    QString usless_str;
    foreach (XString *str, pinterface->StringList) {
        //qDebug()<<img->ID;
       if(!isCheckStrUse(str->ID))
       {
            qDebug()<<"------------FAIL----------"<<str->ID;
            pinterface->removeStr(str);
            count++;
            usless_str.append(str->ID).append("\r\n");
       }
    }
    temp.sprintf("清理字符串 %d/语言 个(usless_str.txt)", count);
    ui->lb_tip->setText(temp);
    pinterface->pSetting->saveFile("./usless_str.txt", usless_str, true);
}

void DialogClean::on_ptn_image_clicked()
{
    int count = 0;
    QString temp;
    QString usless_img;
    foreach (XImg *img, pinterface->ImgList) {
        //qDebug()<<img->ID;
       if(!isCheckImgeUse(img->ID))
       {
            qDebug()<<"------------FAIL----------"<<img->ID;
            pinterface->removeImg(img);
            count++;
            usless_img.append(img->ID).append("\r\n");
       }
    }
    temp.sprintf("清理图片 %d 张(usless_img.txt)", count);
    ui->lb_tip->setText(temp);
    pinterface->pSetting->saveFile("./usless_img.txt", usless_img, true);
}

bool DialogClean::isCheckImgeUse(QString imgID)
{
    QString tempID;
    foreach(Menu_Wnd * wnd, pinterface->menuWndList)
    {
        tempID = pinterface->getWinIamgeId(wnd, ST_NOMAL);
        if(tempID == imgID)
        {
            return true;
        }
        tempID = pinterface->getWinIamgeId(wnd, ST_FOCUS);
        if(tempID == imgID)
        {
            return true;
        }
        tempID = pinterface->getWinIamgeId(wnd, ST_DISABLE);
        if(tempID == imgID)
        {
            return true;
        }
    }
    return false;
}

bool DialogClean::isCheckStrUse(QString strID)
{
    QString tempID;
    foreach(Menu_Wnd * wnd, pinterface->menuWndList)
    {
        tempID = pinterface->getWinStrId(wnd);
        if(tempID == strID)
        {
            return true;
        }

    }
    return false;
}

