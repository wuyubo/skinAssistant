#ifndef XMLEXAMPLE_H
#define XMLEXAMPLE_H

#define FILENAME "xmlfile.xml"
#include "global.h"
#include <QDebug>
#include <QMainWindow>
#include <QFile>
#include <QXmlStreamReader>
#include <QTreeWidgetItem>
#include "zxml.h"
#include <QStandardItem>

namespace Ui {
class xmlExample;
}

typedef struct Menu_Wnd{
    QString frame;
    QDomNode node;
    QTreeWidgetItem *item;
    struct Menu_Wnd *parent;
    struct Menu_Wnd *firstChild;
    struct Menu_Wnd *lbroher;
    struct Menu_Wnd *rbroher;
    bool isMainFrame;
}XMenu_Wnd;

typedef struct IMG{
    QString ID;
    QString FileName;
    QString FolderName;
}XImg;

typedef enum CopyType
{
    CNONE = 0,
    CSOME,
    CALL,
}CType;

typedef struct CopyNode{
    CType type;
    QDomNode node;
    bool isText;
    bool isDisplay;
    bool isAdvanced;
    QString Name;
    XPostoin position;
    XText   text;
    XNavigation navigation;
    XStaticWndProperties properties;
}CNode;


class xmlExample : public QMainWindow
{
  Q_OBJECT

public:
  explicit xmlExample(QWidget *parent = 0);
  ~xmlExample();

  void initCopyNode();
  void initTreeWidget();
  void updateParentItem(QTreeWidgetItem *item);
  void GetWndList(QString frame, QDomNode node);
  void GetGWnd(QDomNode node);
  void showWndlist();
  Menu_Wnd * getlbrother(Menu_Wnd *curwnd, Menu_Wnd *prewnd);
  Menu_Wnd * getMenu_node(QTreeWidgetItem * item);
  void showWndAttr(Menu_Wnd *wnd);
  Menu_Wnd *getParentWnd(QString parenName, Menu_Wnd *prevWnd);
  void setWndAttr(Menu_Wnd *wnd);
  void cloneWndAttr(Menu_Wnd *wnd, CNode cnode);
  void copyWndAttr(Menu_Wnd *wnd);
  Menu_Wnd * add_xmlnode(Menu_Wnd *wnd, Menu_Wnd *copywnd, CopyNode *pcnode, ADD_NODE_TYPE type = ADD_NEW_BEFORE);
  void showtips(QString tips);
  bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
  QDomElement getChildNode(QDomElement node, QString childName);
  void removeChild(Menu_Wnd *wnd);
  void parseXml();
  void getImageList(QDomElement enode);
  XImg *getIamge(QDomNode node);
  void showImge();
  void showIcon(XImg *img);
  void showIconFromId(QString ID);

private slots:
    void on_ptn_read_clicked();

    void on_ptn_write_clicked();
    void treeItemChanged(QTreeWidgetItem *item, int column);

    void on_ptn_setitem_clicked();

    void on_cb_Frame_currentIndexChanged(int index);

    void on_menu_tree_itemClicked(QTreeWidgetItem *item, int column);

    void on_ptn_copy_clicked();

    void on_ptn_pase_attr_clicked();

    void on_ptn_insert_clicked();

    void on_ptn_moveup_clicked();

    void on_ptn_movedowm_clicked();

    void on_ptn_delete_clicked();

    void on_ptn_addimg_clicked();

    void on_lv_img_list_clicked(const QModelIndex &index);

private:
  Ui::xmlExample *ui;
  ZXml xmlContents;
  QXmlStreamReader *xmlReader;
  XmlNumberInfo* pnumberInfo;
  XSkinToolProject *skintoolprj;
  QDomElement m_docElem;
  QList<Menu_Wnd *> menuWndList;
  QList<XImg *>  ImgList;
  CNode m_CopyNode;
  QString filepath;
  QString imgRootPath;
};

#endif // XMLEXAMPLE_H
