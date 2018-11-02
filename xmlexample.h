#ifndef XMLEXAMPLE_H
#define XMLEXAMPLE_H

#define FILENAME "xmlfile.xml"
#include "global.h"
#include "setting.h"
#include "interface.h"
#include "dialogui.h"
#include "dialogimageclone.h"
#include "dialogclean.h"
#include <QDebug>
#include <QMainWindow>
#include <QFile>
#include <QXmlStreamReader>
#include <QTreeWidgetItem>
#include "zxml.h"
#include <QStandardItem>
#include <QLabel>

namespace Ui {
class xmlExample;
}




class xmlExample : public QMainWindow
{
  Q_OBJECT

public:
  explicit xmlExample(QWidget *parent = 0);
  ~xmlExample();

  void initCopyNode();
  void initTreeWidget();
  void updateParentItem(QTreeWidgetItem *item);
  void showWndlist();
  void showWndAttr(Menu_Wnd *wnd);
  void showtips(QString tips);
  XImg *getIamge(QDomNode node);
  void showImge();
  void showIcon(XImg *img, QLabel *lb_icon);
  void showIconFromId(QString ID, QLabel *lb_icon);

  void set_mode(OPTION_MODE mode);

  void showCloneStaticWndProperties(Menu_Wnd *show_wnd, Menu_Wnd *wnd);
  void showWndToUi(Menu_Wnd *wnd);
  void hideWndFromUi(Menu_Wnd *wnd);
  void refreshWndFromUi(Menu_Wnd *wnd);
  void showTreetoUI(Menu_Wnd *wnd);
  void hideTreeFromUI(Menu_Wnd *w);
  void clearCopy();
  void  copyWndAttr(Menu_Wnd *wnd);
  void  setWndAttr(Menu_Wnd *wnd);

public:
  void on_ptn_alignleft_clicked();

  void on_ptn_aligncenter_clicked();

  void on_ptn_alignright_clicked();

  void on_ptn_alignup_clicked();

  void on_ptn_aligncenter_UD_clicked();

  void on_ptn_aligndown_clicked();
  void on_ptn_copy_tree_clicked();
  void on_ptn_conver_clicked();

private slots:

    void treeItemChanged(QTreeWidgetItem *item, int column);

    void on_ptn_setitem_clicked();

    void on_menu_tree_itemClicked(QTreeWidgetItem *item, int column);

    void on_ptn_copy_clicked();

    void on_ptn_pase_attr_clicked();

    void on_ptn_insert_clicked();

    void on_ptn_moveup_clicked();

    void on_ptn_movedowm_clicked();

    void on_ptn_delete_clicked();

    void on_ptn_addimg_clicked();

    void on_lv_img_list_clicked(const QModelIndex &index);

    void on_ptn_insert_after_clicked();

    void on_ptn_insert_child_clicked();

    void on_ptn_showIcon_clicked();

    void on_ptn_showIcon_2_clicked();

    void on_ptn_showIcon_3_clicked();



    void on_ptn_pase_attr_tree_clicked();

    void on_actionAbout_triggered();

    void on_cb_Frame_currentIndexChanged(const QString &arg1);



    void on_ptn_refresh_clicked();



    void on_ptn_moveleft_pos_clicked();

    void on_ptn_moveright_pos_clicked();

    void on_ptn_moveup_pos_clicked();

    void on_ptn_movedown_pos_clicked();

    void on_action_triggered();

    void on_action_save_triggered();

    void on_action_switch_triggered();

    void on_action_align_left_triggered();

    void on_action_alignright_triggered();

    void on_action_alignCenter1_triggered();

    void on_action_alignUp_triggered();

    void on_action_alignCenter2_triggered();

    void on_action_alighDown_triggered();


    void on_action_postion_triggered();

    void on_action_size_triggered();
    void on_action_imageClone_triggered();

    void on_action_clean_triggered();

    void on_ptn_show_hide_clicked();

signals:
    void refreshUI_sig();
private:
  Ui::xmlExample *ui;
  InterFace *pinterface;

  QXmlStreamReader *xmlReader;
  XmlNumberInfo* pnumberInfo;
  XSkinToolProject *skintoolprj;


  CNode m_CopyNode;
  Menu_Wnd *copyTree;
  QString oldFrame;
  OPTION_MODE opt_mode;

  DialogUI *pDialogUI;
  DialogImageClone *pDialogImageClone;
  DialogClean *pDialogClean;

};

#endif // XMLEXAMPLE_H
