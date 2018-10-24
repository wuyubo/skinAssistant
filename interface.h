#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include "global.h"
#include "setting.h"
#include "interface.h"
#include "xlabel.h"
#include <QDebug>
#include <QMainWindow>
#include <QFile>
#include <QXmlStreamReader>
#include <QTreeWidgetItem>
#include "zxml.h"
#include <QStandardItem>
#include <QLabel>

typedef struct Menu_Wnd{
    QString frame;
    QDomNode node;
    QTreeWidgetItem *item;
    struct Menu_Wnd *parent;
    struct Menu_Wnd *firstChild;
    struct Menu_Wnd *lbroher;
    struct Menu_Wnd *rbroher;
    bool isMainFrame;
    XLabel *label;
    bool isShow;
}XMenu_Wnd;

typedef struct IMG{
    QDomNode imgNode;
    QString ID;
    QString FileName;
    QString FolderName;
}XImg;

typedef struct STRING{
    QDomNode imgNode;
    QString ID;
    QString String;
    QString languageID;
    QString languageName;
}XString;

typedef enum CopyType
{
    CNONE = 0,
    CSOME,
    CALL,
    CALIGN,
    CMAX,
}CType;

typedef struct CopyNode{
    CType type;
    QDomNode node;
    bool isText;
    bool isDisplay;
    bool isAdvanced;
    bool isAlign;
    QString Name;
    XPostoin position;
    XText   text;
    XNavigation navigation;
    XStaticWndProperties properties;
    ALIGN_MODE Align;
}CNode;

class InterFace : public QObject
{
    Q_OBJECT
public:
    explicit InterFace(QObject *parent = 0);
    Setting *getSetting();

    ////////////////
    bool read_xml();
    bool load_xml(QString xmlfile);
    bool saveXml();
    void parseXml();
    //////////
    void GetGWnd(QDomNode node);
    void GetWndList(QString frame, QDomNode node);
    void getImageList(QDomElement enode);
    XImg *getIamge(QDomNode node);
    void getLanguageList(QDomElement enode);
    void getStringList(QDomElement enode, QString languageId,QString languageName);
    void setCurframe(QString frame);
    QString getCurframe();
    /////////
    QString getString(QString Id, QString languageName);
    Menu_Wnd * getMenu_node(QTreeWidgetItem * item);
    /////////
    void appendMenuWndList(Menu_Wnd * wnd);
    void appendImgList(XImg * img);
    void appendStringList(XString * string);
    void appendFrameList(QString frame);
    /////////
    bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
    QString getRootPathFromxmlFile(QString xmlfile);
    /////////
    Menu_Wnd * add_xmlnode(Menu_Wnd *wnd, Menu_Wnd *copywnd, CopyNode *pcnode, ADD_NODE_TYPE type);
    IMG *add_xmlImg(IMG *lastImg, IMG *tagImg);
    bool checkImg(QString imgName);
    bool addImge(QString imgPath);
    Menu_Wnd * getParentWnd(QString parenName, Menu_Wnd *prevWnd);
    Menu_Wnd * getlbrother(Menu_Wnd *curwnd, Menu_Wnd *prewnd);
    Menu_Wnd* getWndLastChild(Menu_Wnd *wnd);
    QDomElement getChildNode(QDomElement node, QString childName);
    void removeChild(Menu_Wnd *wnd);
    Menu_Wnd *getWndFromId(QString ID);
    /////////////
   // void cloneNode(QDomNode node, QDomNode cnode);
   // void cloneTree(Menu_Wnd * wnd, Menu_Wnd * ctree);
    void  cloneWndAttr(Menu_Wnd *wnd, CNode cnode, CNode _CopyNode);
    ////////////////

signals:

public slots:
public:
    Setting *pSetting;

    QString _xml_file;
    ZXml xmlContents;

    QDomElement m_docElem;

    QList<Menu_Wnd *> menuWndList;
    QList<XImg *>  ImgList;
    QList<XString *>  StringList;
    QStringList frameList;
    QString curframe;
};

#endif // INTERFACE_H
