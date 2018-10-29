#include "dialogimageclone.h"
#include "ui_dialogimageclone.h"
#include <QDir>

DialogImageClone::DialogImageClone(InterFace *_pinterface, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImageClone)
{
    ui->setupUi(this);
    pinterface = _pinterface;
}

DialogImageClone::~DialogImageClone()
{
    delete ui;
}

void DialogImageClone::on_pushButton_clicked()
{
    int count = 0;
    QString temp;
    foreach (XImg *img, pinterface->ImgList) {
        if(CloneImage(img))
        {
            count++;
        }
    }
    temp.sprintf("克隆完成: 成功 %d 张，失败 %d 张！", count,pinterface->ImgList.length()-count);
    ui->lb_tip->setText(temp);
}
bool DialogImageClone::CloneImage(XImg *img)
{
    QString filepath;
    QString folder = img->FolderName.replace("\\", "/");
    QString temp = folder.left(6);
    QString temp2 = folder.left(2);
    QString targetPathRoot = ui->le_targePath->text();
    QString targetPath;
    QString targetName;
    QString targetFolder;
    QString suffix = ui->le_suffix->text()+".";
    QDir dir;
    targetName = img->FileName;
    targetName.replace(".", suffix);
    targetPathRoot.replace("\\", "/");

    if(temp == "../../")
    {
        folder.replace("../../bitmap1366x768x565", "");
        folder.replace(pinterface->pSetting->getImgRelativePath(), "");
        folder.replace("../../", "/../");
    }else if(temp2 == "./")
    {
        folder.replace(pinterface->pSetting->getImgRelativePath(), "");
    }

    filepath = pinterface->pSetting->getImgPath()+folder+"/"+img->FileName;
    targetFolder = targetPathRoot+folder;
    targetPath = targetPathRoot+folder+"/"+targetName;
    //qDebug() << filepath;
    //qDebug() << targetPath;


    //dir.cd(targetPathRoot);  //进入某文件夹
    if(!dir.exists(targetFolder))//判断需要创建的文件夹是否存在
    {
        //qDebug() << targetFolder;
        dir.mkdir(targetFolder); //创建文件夹
    }
   return pinterface->copyFileToPath(filepath, targetPath, true);
}

void DialogImageClone::on_pushButton_2_clicked()
{
    QString data;
    QString suffix = ui->le_suffix->text();
    data.append("array[][2] = {\r\n");
    foreach (XImg *img, pinterface->ImgList) {
        data.append("{E_BMP_").append(img->ID).append(",");
        data.append("E_BMP_").append(img->ID).append(suffix).append("},\r\n");
    }
    data.append("};\r\n");
    qDebug()<<data;
    if(pinterface->pSetting->saveFile(pinterface->pSetting->getRootPath()+"array.c", data, true))
    {
        ui->lb_tip->setText("生成二维数据成功");
    }
}
