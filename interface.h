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
#include <QCompleter>
#include <QStringListModel>

typedef enum{
    ST_FOCUS,
    ST_NOMAL,
    ST_DISABLE,
}ITEM_STATUS;
typedef struct Menu_Wnd{
    QString frame;
    QDomNode node;
    QTreeWidgetItem *item;
    struct Menu_Wnd *parent;
    struct Menu_Wnd *firstChild;
    struct Menu_Wnd *lbroher;
    struct Menu_Wnd *rbroher;
    bool isMainFrame;
    QLabel *label;
    bool isShow;
}XMenu_Wnd;

typedef struct IMG{
    QDomNode imgNode;
    QString ID;
    QString FileName;
    QString FolderName;
}XImg;

typedef struct STRING{
    QDomNode strNode;
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
    void init();
    Setting *getSetting();

    ////////////////
    bool read_xml();
    bool load_xml(QString xmlfile);
    bool saveXml();
    void parseXml();
    //////////
    void GetGWnd(QDomNode node);
    Menu_Wnd *GetMainFrameWnd(QString frame, QDomNode node);
    Menu_Wnd *GetNormalWnd(QString frame, QDomNode node, Menu_Wnd *lastWnd);
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
    void setWidth(int w);
    int getWidth();
    void setHeight(int h);
    int getHeight();
    /////////////
   // void cloneNode(QDomNode node, QDomNode cnode);
   // void cloneTree(Menu_Wnd * wnd, Menu_Wnd * ctree);
    void  cloneWndAttr(Menu_Wnd *wnd, CNode cnode, CNode _CopyNode);
    QString getWinIamgeId(Menu_Wnd *wnd, ITEM_STATUS status);
    void setWinIamgeId(Menu_Wnd *wnd, QString imgID, ITEM_STATUS status);
    QString checkImgFromID(QString imgID, int type = 0);
    bool removeImg(XImg *img);
    QString getWinStrId(Menu_Wnd *wnd);
    void setWinStrId(Menu_Wnd *wnd, QString imgID);
    bool removeStr(XString *str);
    QString getWinName(Menu_Wnd *wnd);
    QString getWinParentName(Menu_Wnd *wnd);
    ////////////////
    QString getWinTextId(Menu_Wnd *wnd, ITEM_STATUS status);
    void setWinTextId(Menu_Wnd *wnd, QString txtID, ITEM_STATUS status);
    ////////////////
    void initCompleter();
    Menu_Wnd *getWndByName(QString name);
signals:

public slots:
public:
    Setting *pSetting;

    QString _xml_file;
    ZXml xmlContents;

    QDomElement m_docElem;
    QDomNode SizePanel;
    QDomNodeList GWndList;

    QList<Menu_Wnd *> menuWndList;
    QList<XImg *>  ImgList;
    QList<XString *>  StringList;
    QStringList frameList;
    QString curframe;
    int ui_width;
    int ui_height;

    Menu_Wnd *curFrameWnd;

    QStringList strWndList;
    QCompleter *pCompleter;
//    QStringListModel *pStrListModel;
};

#endif // INTERFACE_H
