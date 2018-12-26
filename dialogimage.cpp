#include "dialogimage.h"
#include "ui_dialogimage.h"
#include <QFileDialog>

DialogImage::DialogImage(InterFace *_pinterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImage)
{
    ui->setupUi(this);
    pinterface = _pinterface;
    curIdx = -1;
    ui->le_end_color->setEnabled(false);
    ui->le_from_color->setEnabled(false);
    pDialogImageClone = new DialogImageClone(_pinterface, this);
}

DialogImage::~DialogImage()
{
    delete ui;
}

void DialogImage::showImgeList()
{
    imgList.clear();
    showList.clear();
    foreach(XImg * img, pinterface->ImgList)
    {
        imgList.append(img->ID);
        showList.append(img->ID);
    }

    updateImgList(imgList);
}

void DialogImage::updateImgList(QStringList strList)
{
    int nCount = strList.size();
    QString countText;
    countText.sprintf("一共有图片%d张", nCount);
    QStandardItemModel *standardItemModel = new QStandardItemModel(this);
    for(int i = 0; i < nCount; i++)
    {
        QString string = static_cast<QString>(strList.at(i));
        QStandardItem *item = new QStandardItem(string);
        if(i % 2 == 1)
        {
            QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
            linearGrad.setColorAt(0, Qt::gray);
            QBrush brush(linearGrad);
            item->setBackground(brush);
        }
        standardItemModel->appendRow(item);
    }
    ui->lb_count->setText(countText);
    ui->lv_image->setModel(standardItemModel);
}

void DialogImage::on_ptn_addimage_clicked()
{
    QFileDialog fileDialog(this);
    QStringList filters;
    QStringList strPathList;
    fileDialog.setWindowTitle(tr("请选择图片"));
    filters << "Image files (*.bmp *.png)"<< "Any files (*)";
    fileDialog.setNameFilters(filters);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    if (fileDialog.exec() == QDialog::Accepted)
    {
        strPathList = fileDialog.selectedFiles();
    }

    foreach (QString fileName, strPathList) {
        if(pinterface->addImge(fileName))
        {
            //DEBUG(fileName);
        }

    }

    showImgeList();

}

void DialogImage::on_lineEdit_editingFinished()
{

    on_ptn_search_clicked();
}

void DialogImage::on_ptn_search_clicked()
{
       QString serchtext = ui->lineEdit->text();
       showList.clear();
       foreach(QString word, imgList)
       {
            if(word.contains(serchtext,Qt::CaseInsensitive))
            {
                showList.append(word);
               // DEBUG(word);
            }
       }
       updateImgList(showList);
}

void DialogImage::on_lineEdit_cursorPositionChanged(int arg1, int arg2)
{
    arg1=arg1;
    arg2=arg2;
    on_ptn_search_clicked();
}

void DialogImage::on_lv_image_clicked(const QModelIndex &index)
{
    XImg *img;
    QString id = showList.at(index.row());

    int imgIdx = 0;
    foreach (QString imgId, imgList) {
        if(imgId == id )
        {
            break;
        }
        imgIdx++;
    }
    curIdx = imgIdx;
    img = pinterface->ImgList.at(imgIdx);
    showImgAttr(img);

    showIcon(img,ui->lb_img);
}

void DialogImage::showIcon(XImg *img, QLabel *lb_icon)
{
    QString filepath;
    QString folder = img->FolderName.replace("\\", "/");
    QString temp = folder.left(6);
    qDebug() << img->FolderName;
    if(temp == "../../")
    {
        filepath = pinterface->pSetting->getRootPath()+folder+"/"+img->FileName;
    }else
    {
        filepath = pinterface->pSetting->getImgPath()+img->FolderName.replace("\\", "/")+"/"+img->FileName;
    }
   // qDebug() << filepath;
    QPixmap src(filepath);//QPixmap src;   src.load(fileName);
    lb_icon->setPixmap(src);//若改变图像适应label，则 ui->label->setPixmap(src.scaled(ui->label->size()));
}
bool DialogImage::openImageXml()
{
    QFile file("IMAGE.XML");
    if (!file.open(QFile::WriteOnly)) {
      qDebug() << "Cannot read file";
      return false;
    }
    imgdocElem = imgXmlContents.xmlRead(&file);
    file.close();
    return true;
}

void DialogImage::createImageList()
{
    QDomElement imgListNode = imgXmlContents.xmlCreateNode("ImageList");
    imgdocElem.appendChild(imgListNode);
}

XImg * DialogImage::cloneImgNode(QDomElement srcNode, QString fileSuffix, QString forderPrefix)
{
    const int atr_count = 8;
    QString atrValue;
    XImg * newImg = new XImg();
    QString attr_Str[atr_count] =
    {
        "ID",
        "CvtFormat",
        "AlphaFrom",
        "FileType",
        "ColorKeyFrom",
        "ColorKeyEnd",
        "EnableClrKey",
        "EnableDither"
    };
    QDomElement tagNode = pinterface->xmlContents.xmlCreateNode("Image");

    for(int i=0 ;i < atr_count; i++) {
        atrValue = srcNode.attribute(attr_Str[i]);
        if(i == 0)
        {
            atrValue += fileSuffix;
            newImg->ID = atrValue;
        }
        QDomAttr xmlAttr = pinterface->xmlContents.xmlCreateAttribute(attr_Str[i]);
        xmlAttr.setValue(atrValue);
        tagNode.setAttributeNode(xmlAttr);
    }

    DEBUG("==================");

    QDomNode n = srcNode.firstChild();
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();
           if(e.tagName () == "FileName")
           {
               QDomElement element_FileName = pinterface->xmlContents.xmlCreateNode("FileName");
               QString txt = e.text();
               QStringList templist = txt.split(".");
               txt = templist[0]+fileSuffix+"."+templist[1];
               element_FileName.appendChild(pinterface->xmlContents.xmlCreateTextNode(txt));
               tagNode.appendChild(element_FileName);
               newImg->FileName = txt;
           }
           if(e.tagName () == "FolderName")
           {
               QDomElement element_FolderName = pinterface->xmlContents.xmlCreateNode("FolderName");
               QString txt = e.text();
               if(txt == "..\\..\\bitmap1366x768x565")
               {
                   DEBUG(txt);
                   txt="";
               }
               element_FolderName.appendChild(pinterface->xmlContents.xmlCreateTextNode(forderPrefix+txt));
               tagNode.appendChild(element_FolderName);
               newImg->FolderName = forderPrefix+txt;
           }
       }
        n = n.nextSibling();
    }
    newImg->imgNode = tagNode;
    return newImg;
}


void DialogImage::on_ptn_mirror_clicked()
{
    XImg *lastImg = pinterface->ImgList.last();
    uint16_t count = pinterface->ImgList.length();
    QDomNode imgListNode = lastImg->imgNode.parentNode();
    for(uint16_t i = 0; i < count; i++)
    {
        XImg * img = pinterface->ImgList.at(i);
        XImg * newImg = cloneImgNode(img->imgNode.toElement(), "_mirror", "_mirror");
        imgListNode.appendChild(newImg->imgNode);
        pinterface->ImgList.append(newImg);
    }
    showImgeList();
    ui->ptn_mirror->setEnabled(false);
    ui->lb_tips->setText("镜像图片添加成功，请确保本地镜文件已经生成");
}

void DialogImage::on_ptn_delete_clicked()
{
    XImg *img = pinterface->ImgList.at(curIdx);
    img->imgNode.parentNode().removeChild(img->imgNode);
    pinterface->ImgList.removeAt(curIdx);
    delete img;
    showImgeList();
}

void DialogImage::showImgAttr(XImg *img)
{
    QDomElement e=img->imgNode.toElement();
    bool ok;
    int value;
    QString strvalue;
    ui->le_ID_name->setText(img->ID);


    value= e.attribute("EnableDither").toInt(&ok);
    if(value == 1)
    {
        ui->cb_RGB->setCheckState(Qt::Checked);
    }else
    {
        ui->cb_RGB->setCheckState(Qt::Unchecked);
    }

    value = e.attribute("FileType").toInt(&ok);
    if(value == 1)
    {
        ui->cb_Alpha->setCheckState(Qt::Checked);
    }else
    {
        ui->cb_Alpha->setCheckState(Qt::Unchecked);
    }

    value = e.attribute("EnableClrKey").toInt(&ok);
    if(value == 1)
    {
        ui->cb_Color->setCheckState(Qt::Checked);
    }else
    {
        ui->cb_Color->setCheckState(Qt::Unchecked);
    }

    strvalue = e.attribute("ColorKeyFrom");
    ui->le_from_color->setText(strvalue);

    strvalue = e.attribute("ColorKeyEnd");
    ui->le_end_color->setText(strvalue);

    value = e.attribute("AlphaFrom").toInt(&ok);
    ui->combAlpha->setCurrentIndex(value);
    value= e.attribute("CvtFormat").toInt(&ok);
    ui->combPixel->setCurrentIndex(value);


}

void DialogImage::on_cb_Color_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->le_end_color->setEnabled(false);
        ui->le_from_color->setEnabled(false);
    }else
    {
        ui->le_end_color->setEnabled(true);
        ui->le_from_color->setEnabled(true);
    }
}

void DialogImage::on_pushButton_clicked()
{
    XImg *img = pinterface->ImgList.at(curIdx);
    QDomElement e=img->imgNode.toElement();
    if(ui->cb_RGB->isChecked())
    {
        e.setAttribute("EnableDither", "1");
    }else
    {
        e.setAttribute("EnableDither", "0");
    }
    if(ui->cb_Alpha->isChecked())
    {
        e.setAttribute("FileType", "1");
    }else
    {
        e.setAttribute("FileType", "0");
    }

    if(ui->cb_Color->isChecked())
    {
        e.setAttribute("EnableClrKey", "1");
    }else
    {
        e.setAttribute("EnableClrKey", "0");
    }
    e.setAttribute("ColorKeyFrom", ui->le_from_color->text());
    e.setAttribute("ColorKeyEnd", ui->le_end_color->text());

    e.setAttribute("AlphaFrom", ui->combAlpha->currentIndex());
    e.setAttribute("CvtFormat", ui->combPixel->currentIndex());

}

void DialogImage::on_ptn_clone_clicked()
{
    pDialogImageClone->show();
}
