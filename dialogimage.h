#ifndef DIALOGIMAGE_H
#define DIALOGIMAGE_H

#include <QDialog>
#include "global.h"
#include "interface.h"
#include "dialogimageclone.h"

namespace Ui {
class DialogImage;
}

class DialogImage : public QDialog
{
    Q_OBJECT

public:
    explicit DialogImage(InterFace *_pinterface, QWidget *parent = 0);
    ~DialogImage();
    void showImgeList();
    void updateImgList(QStringList strList);
    void showIcon(XImg *img, QLabel *lb_icon);
    bool openImageXml();
    void createImageList();
    XImg *cloneImgNode(QDomElement srcNode, QString fileSuffix = "", QString forderPrefix = "");
    void showImgAttr(XImg *img);

private slots:
    void on_ptn_addimage_clicked();

    void on_lineEdit_editingFinished();

    void on_ptn_search_clicked();

    void on_lineEdit_cursorPositionChanged(int arg1, int arg2);

    void on_lv_image_clicked(const QModelIndex &index);

    void on_ptn_mirror_clicked();

    void on_ptn_delete_clicked();

    void on_cb_Color_stateChanged(int arg1);

    void on_pushButton_clicked();

    void on_ptn_clone_clicked();

private:
    Ui::DialogImage *ui;
    InterFace *pinterface;
    QStringList imgList;
    QStringList showList;

    QDomElement imgdocElem;
    ZXml imgXmlContents;
    int curIdx;
    DialogImageClone *pDialogImageClone;
};

#endif // DIALOGIMAGE_H
