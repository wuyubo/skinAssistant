#include "xmlexample.h"
#include "ui_xmlexample.h"
#include "xlabel.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractItemView>
#include <thread>


xmlExample::xmlExample(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::xmlExample)
{
  ui->setupUi(this);

  xmlReader = NULL;
  oldFrame = "";
  initCopyNode();
  initTreeWidget();
  set_mode(MODE_SIMPLE);
  //this->showMaximized();
  pinterface = new InterFace(this);
  pDialogUI  = new DialogUI(pinterface, parent);
  pDialogClean = new DialogClean(pinterface, this);
  pDialgImgManager = new DialogImage(pinterface, this);
  pinterface->setCurframe(ui->cb_Frame->currentText());
  connect(ui->menu_tree,SIGNAL(itemChanged(QTreeWidgetItem *,int)),this,SLOT(treeItemChanged(QTreeWidgetItem*,int)));
  connect(this,SIGNAL(refreshUI_sig()),pDialogUI,SLOT(refreshUI()));
}

xmlExample::~xmlExample()
{
  delete ui;
}

void xmlExample::initCopyNode()
{
    m_CopyNode.type = CNONE;
    m_CopyNode.isAdvanced = false;
    m_CopyNode.isText = false;
    m_CopyNode.isDisplay = false;
    m_CopyNode.Name = "";
}


void xmlExample::showWndlist()
{
    QTreeWidgetItem *group1;
    ui->menu_tree->clear();
    pinterface->setCurframe(ui->cb_Frame->currentText());

    if(oldFrame != "")
    {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->frame == oldFrame)
            {
                group1 = wnd->item;
                if(group1)
                {
                    wnd->item = NULL;
                }
            }
        }
        oldFrame = "";
    }

    foreach(Menu_Wnd * wnd, pinterface->menuWndList)
    {
        if(wnd->frame != pinterface->getCurframe())
        {
            continue;
        }

        if(wnd->node.toElement().attribute("CtrlTypeName") == "Main Frame")
        {
             group1=new QTreeWidgetItem(ui->menu_tree);
             group1->setText(0,wnd->node.toElement().attribute("Name"));
             group1->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
             group1->setCheckState(0,Qt::Unchecked);
             wnd->item = group1;
        }
        else
        {
             QTreeWidgetItem *item11=new QTreeWidgetItem(wnd->parent->item);
             item11->setText(0,wnd->node.toElement().attribute("Name"));
             item11->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
             item11->setCheckState(0,Qt::Unchecked);
             wnd->item = item11;
        }
    }
}




void xmlExample::showImge()
{
    QStringList strList;
    foreach(XImg * img, pinterface->ImgList)
    {
        strList.append(img->ID);
    }

    int nCount = strList.size();
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
    ui->lv_img_list->setModel(standardItemModel);
}

//initTreeWidget() 的实现
void xmlExample::initTreeWidget()
{
    ui->menu_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->menu_tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->menu_tree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->menu_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->menu_tree->header()->setStretchLastSection(false);
   // listWidget.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection) #设置选择模式

   // 选择模式有:ExtendedSelection 按住ctrl多选, SingleSelection 单选 MultiSelection 点击多选 ContiguousSelection 鼠标拖拉多选
}

//treeItemChanged(QTreeWidgetItem *item, int column)的实现
void xmlExample::treeItemChanged(QTreeWidgetItem *item, int column)
{
    Menu_Wnd *wnd = pinterface->getMenu_node(item);
    column = column;
    if(wnd == NULL)
    {
        return;
    }
    if(item->checkState(0) == Qt::Checked)
    {
        wnd->isShow = true;
    }else
    {
        wnd->isShow = false;
    }
}

//updateParentItem(QTreeWidgetItem *item) 的实现
void xmlExample::updateParentItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem* parent=item->parent();
    if(parent==NULL)
    {
        return;
    }

    //选中的子节点个数
   /* int selectCount=0;
    int childCount=parent->childCount();
    for(int i=0;i<childCount;i++)
    {
        QTreeWidgetItem *childItem=parent->child(i);
        if(childItem->checkState(0)==Qt::Checked)
        {
            selectCount++;
        }
    }
    if(selectCount<=0)
    {
        //选中状态
        parent->setCheckState(0,Qt::Unchecked);
    }
    else if(selectCount>0 && selectCount<childCount)
    {
        //部分选中状态
        parent->setCheckState(0,Qt::PartiallyChecked);
    }
    else if(selectCount==childCount)
    {
        //未选中状态
        parent->setCheckState(0,Qt::Checked);
    }*/
}



void xmlExample::on_ptn_setitem_clicked()
{
     QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
     if(currentItem==Q_NULLPTR)
     {
         return;
     }

     Menu_Wnd *wnd = pinterface->getMenu_node(currentItem);
     if(wnd)
     {
         setWndAttr(wnd);
         currentItem->setText(0, ui->le_name->text());
     }
     showtips("修改属性成功");
     updateUI();
}


void xmlExample::on_menu_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    Menu_Wnd *wnd = pinterface->getMenu_node(item);
    column = column;
    if(wnd)
    {
        showWndAttr(wnd);
        updateUI();
    }
}





void xmlExample::showWndAttr(Menu_Wnd *wnd)
{
    QDomNode n=wnd->node.firstChild();
    bool ok;
    ui->le_name->setText(wnd->node.toElement().attribute("Name"));
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position")
           {
                ui->le_XPos->setText(e.toElement().attribute("X"));
                ui->le_YPos->setText(e.toElement().attribute("Y"));
                ui->le_Width->setText(e.toElement().attribute("Width"));
                ui->le_Height->setText(e.toElement().attribute("Height"));
                ui->lb_img->resize(e.toElement().attribute("Width").toInt(&ok),e.toElement().attribute("Height").toInt(&ok));
           }
           if(e.tagName () == "Text")
           {
                ui->le_text->setText(e.toElement().attribute("TextID"));
                ui->le_Align->setText(e.toElement().attribute("TextAlign"));
                ui->le_NormColor->setText(e.toElement().attribute("NormalTextColor"));
                ui->le_FocColor->setText(e.toElement().attribute("FocusTextColor"));
                ui->le_disColor->setText(e.toElement().attribute("DisableTextColor"));
                ui->le_font_size_0->setText(e.toElement().attribute("FontTypeIndex"));
                ui->le_font_size_1->setText(e.toElement().attribute("FocusFontTypeIndex"));
                ui->le_font_size_2->setText(e.toElement().attribute("DisableFontTypeIndex"));

                if(ui->le_text->text() != "")
                {
                    ui->lb_img->setText(pinterface->getString(ui->le_text->text(), "English"));
                }
           }
           if(e.tagName () == "StaticWndProperties")
           {
                QString state = e.toElement().attribute("BgState");
                QString color_Nomal = e.toElement().attribute("HasNormalDrawStyle");
                QString color_Focus = e.toElement().attribute("HasFocusDrawStyle");
                QString color_Disble = e.toElement().attribute("HasDisableDrawStyle");
                ui->le_clone->setText(e.toElement().attribute("Clone"));
                ui->le_anomcolor->setText(e.toElement().attribute("NormalBgColor"));
                ui->le_afoccolor->setText(e.toElement().attribute("FocusBgColor"));
                ui->le_adiscolor->setText(e.toElement().attribute("DisabledBgColor"));
                ui->le_nomBitmap->setText(e.toElement().attribute("NormalBitmapID"));
                ui->le_focBitmap->setText(e.toElement().attribute("FocusBitmapID"));
                ui->le_disBitmap->setText(e.toElement().attribute("DisabledBitmapID"));


                if(color_Nomal == "1")
                {
                    ui->cb_bg_nomal->setChecked(true);
                }else
                {
                    ui->cb_bg_nomal->setChecked(false);
                }
                if(color_Focus == "1")
                {
                    ui->cb_bg_focus->setChecked(true);
                }else
                {
                    ui->cb_bg_focus->setChecked(false);
                }
                if(color_Disble == "1")
                {
                    ui->cb_bg_disble->setChecked(true);
                }else
                {
                    ui->cb_bg_disble->setChecked(false);
                }
                ui->lb_img->setPixmap(QPixmap(""));
                if(state == "0")
                {
                    ui->rbn_clone->setChecked(true);
                }else if(state == "1")
                {
                    ui->rbn_color->setChecked(true);
                }
                else if(state == "2")
                {
                    ui->rbn_bitmap->setChecked(true);
                    showIconFromId(e.toElement().attribute("NormalBitmapID"), ui->lb_img);
                }
           }
           if(e.tagName () == "Navigation")
           {
                ui->le_Navigation_up->setText(e.toElement().attribute("Up"));
                ui->le_Navigation_down->setText(e.toElement().attribute("Down"));
                ui->le_Navigation_left->setText(e.toElement().attribute("Left"));
                ui->le_Navigation_right->setText(e.toElement().attribute("Right"));
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}


void xmlExample::on_ptn_copy_clicked()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();

    if(currentItem==Q_NULLPTR)
    {
        return;
    }

    Menu_Wnd *wnd = pinterface->getMenu_node(currentItem);
    if(wnd)
    {
        initCopyNode();
        if(ui->cb_all->isChecked())
        {
           m_CopyNode.type = CALL;
           m_CopyNode.isAdvanced = true;
           m_CopyNode.isText = true;
           m_CopyNode.isDisplay = true;
        }else
        {
            m_CopyNode.type = CSOME;
            if(ui->cb_advanced->isChecked())
            {
                m_CopyNode.isAdvanced = true;
            }
            if(ui->cb_text->isChecked())
            {
                m_CopyNode.isText = true;
            }
            if(ui->cb_display->isChecked())
            {
                m_CopyNode.isDisplay = true;
            }
        }
        m_CopyNode.node = wnd->node;
        m_CopyNode.Name = ui->le_name->text();
        copyWndAttr(wnd);
        if(m_CopyNode.isDisplay)
        {
            if(ui->le_XPos->text() == "")
            {
                 m_CopyNode.position.X = -1;
            }
            if(ui->le_YPos->text() == "")
            {
                 m_CopyNode.position.Y = -1;
            }
            if(ui->le_Width->text() == "")
            {
                 m_CopyNode.position.Width = -1;
            }
            if(ui->le_Height->text() == "")
            {
                 m_CopyNode.position.Height = -1;
            }

        }
    }
    showtips("复制成功");
}

void xmlExample::on_ptn_pase_attr_clicked()
{
    if(m_CopyNode.type == CNONE)
    {
        return;
    }
   QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
   QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
   Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
   if(ui->cb_parse_type->currentText() == "左对齐")
   {
       m_CopyNode.Align = ALIGN_LEFT;
   }else if(ui->cb_parse_type->currentText() == "右对齐")
   {
        m_CopyNode.Align = ALIGN_RIGHT;
   }else if(ui->cb_parse_type->currentText() == "相对父节点对齐")
   {
        m_CopyNode.Align = ALIGN_PARENT;
   }else if(ui->cb_parse_type->currentText() == "设置大小")
   {
        m_CopyNode.Align = ALIGN_RESIZE;
   }
   else
   {
       m_CopyNode.Align = ALIGN_NORMAL;
   }
   foreach (QTreeWidgetItem* item, itemlist) {
       foreach(Menu_Wnd * wnd, pinterface->menuWndList)
       {
           if(wnd->item == item)
           {
               pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
               updateUI();
           }
       }
   }
    showWndAttr(curwnd);
    showtips("粘贴成功");
}



void xmlExample::showtips(QString tips)
{
    ui->statusBar->showMessage(tips);
   // ui->lb_tips->setText(tips);
}


void xmlExample::on_ptn_insert_clicked()
{
    if(m_CopyNode.type == CNONE)
    {
        return;
    }
    Menu_Wnd *nwnd;
    int index;
   QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
   foreach (QTreeWidgetItem* item, itemlist) {
       foreach(Menu_Wnd * wnd, pinterface->menuWndList)
       {
           if(wnd->item == item)
           {
               nwnd = pinterface->add_xmlnode(wnd, wnd, &m_CopyNode, ADD_NEW_BEFORE);
               QTreeWidgetItem *item=new QTreeWidgetItem(0);
               item->setText(0,nwnd->node.toElement().attribute("Name"));
               item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
               item->setCheckState(0,Qt::Unchecked);
               index = wnd->parent->item->indexOfChild(wnd->item);
               wnd->parent->item->insertChild(index, item);
               nwnd->item = item;
           }
       }
   }
   showtips("插入成功");
}

void xmlExample::on_ptn_moveup_clicked()
{
    Menu_Wnd *curwnd;
    Menu_Wnd *brownd;
    int index;
    QTreeWidgetItem* tempitem;


    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        tempitem = item;
        curwnd = pinterface->getMenu_node(item);
        brownd = curwnd->lbroher;
        if(brownd != curwnd)
        {
            index = curwnd->parent->item->indexOfChild(brownd->item);
            curwnd->parent->item->removeChild(item);
            curwnd->parent->item->insertChild(index, tempitem);
            curwnd->item = brownd->item;
            brownd->item = tempitem;

            pinterface->add_xmlnode(brownd, curwnd, NULL, ADD_MOVE_UP);
            ui->menu_tree->setItemSelected(item,true);
        }
    }

    showtips("移动成功");

}

void xmlExample::on_ptn_movedowm_clicked()
{
    Menu_Wnd *curwnd;
    Menu_Wnd *brownd;
    int index;
    QTreeWidgetItem* tempitem;

    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        tempitem = item;
        curwnd = pinterface->getMenu_node(item);
        brownd = curwnd->rbroher;
        if(brownd != curwnd)
        {
            index = curwnd->parent->item->indexOfChild(brownd->item);
            curwnd->parent->item->removeChild(item);
            curwnd->parent->item->insertChild(index, tempitem);
            curwnd->item = brownd->item;
            brownd->item = tempitem;

            pinterface->add_xmlnode(brownd, curwnd, NULL, ADD_MOVE_DOWN);
            ui->menu_tree->setItemSelected(item,true);
        }
    }
    showtips("移动成功");
}




void xmlExample::on_ptn_delete_clicked()
{
    Menu_Wnd *curwnd;
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    foreach (QTreeWidgetItem* item, itemlist) {
        curwnd = pinterface->getMenu_node(item);
        pinterface->removeChild(curwnd);
        curwnd->node.parentNode().removeChild(curwnd->node);
        pinterface->menuWndList.removeOne(curwnd);

        if(curwnd->lbroher != curwnd)
        {
            curwnd->lbroher->rbroher = curwnd->rbroher;
            curwnd->rbroher->lbroher = curwnd->lbroher;
        }else if(curwnd->rbroher != curwnd)
        {
            curwnd->rbroher->lbroher = curwnd->rbroher;
        }

        delete curwnd;
        curwnd->parent->item->removeChild(item);
    }
    showtips("删除成功");
}



void xmlExample::on_ptn_addimg_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开图像"), pinterface->pSetting->getImgPath(), tr("*.bmp *.png"));

    if(pinterface->addImge(fileName))
    {
        QPixmap src(fileName);//QPixmap src;   src.load(fileName);
        ui->lb_img->setPixmap(src);//若改变图像适应label，则 ui->label->setPixmap(src.scaled(ui->label->size()));
    }
}

void xmlExample::showIconFromId(QString ID, QLabel *lb_icon)
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

void xmlExample::showIcon(XImg *img, QLabel *lb_icon)
{
    QString filepath;
    if(img->FolderName.replace("\\", "/") == "../../bitmap1366x768x565")
    {
        filepath = pinterface->pSetting->getImgPath()+img->FileName;
    }else
    {
        filepath = pinterface->pSetting->getImgPath()+img->FolderName.replace("\\", "/")+"/"+img->FileName;
    }
    //qDebug() << filepath;
    QPixmap src(filepath);//QPixmap src;   src.load(fileName);

    lb_icon->setPixmap(src.scaled(lb_icon->size()));//若改变图像适应label，则 ui->label->setPixmap(src.scaled(ui->label->size()));
}

void xmlExample::on_lv_img_list_clicked(const QModelIndex &index)
{
    XImg *img = pinterface->ImgList.at(index.row());
    showIcon(img,ui->lb_img);

}

void xmlExample::on_ptn_insert_after_clicked()
{
    if(m_CopyNode.type == CNONE)
    {
        return;
    }
    Menu_Wnd *nwnd;
    int index;
   QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
   foreach (QTreeWidgetItem* item, itemlist) {
       foreach(Menu_Wnd * wnd, pinterface->menuWndList)
       {
           if(wnd->item == item)
           {
               nwnd = pinterface->add_xmlnode(wnd, wnd, &m_CopyNode, ADD_NEW_AFTER);

               QTreeWidgetItem *item=new QTreeWidgetItem(0);
               item->setText(0,nwnd->node.toElement().attribute("Name"));
               item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
               item->setCheckState(0,Qt::Unchecked);
               index = wnd->parent->item->indexOfChild(wnd->item);
               wnd->parent->item->insertChild(index+1, item);
               nwnd->item = item;
           }
       }
   }
   showtips("插入成功");
}

void xmlExample::on_ptn_insert_child_clicked()
{
    if(m_CopyNode.type == CNONE)
    {
        return;
    }
    Menu_Wnd *nwnd;

   QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
   foreach (QTreeWidgetItem* item, itemlist) {
       foreach(Menu_Wnd * wnd, pinterface->menuWndList)
       {
           if(wnd->item == item)
           {
               nwnd = pinterface->add_xmlnode(wnd, wnd, &m_CopyNode, ADD_NEW_CHILD);

               QTreeWidgetItem *item=new QTreeWidgetItem(wnd->item);
               item->setText(0,nwnd->node.toElement().attribute("Name"));
               item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
               item->setCheckState(0,Qt::Unchecked);
               nwnd->item = item;
           }
       }
   }
   showtips("插入成功");
}

void xmlExample::on_ptn_showIcon_clicked()
{
    showIconFromId(ui->le_nomBitmap->text(), ui->lb_img);
}

void xmlExample::on_ptn_showIcon_2_clicked()
{
    showIconFromId(ui->le_focBitmap->text(), ui->lb_img);
}

void xmlExample::on_ptn_showIcon_3_clicked()
{
    showIconFromId(ui->le_disBitmap->text(), ui->lb_img);
}

void xmlExample::on_ptn_copy_tree_clicked()
{
    QTreeWidgetItem * curItem = ui->menu_tree->currentItem();
    copyTree = pinterface->getMenu_node(curItem);

}




void xmlExample::on_ptn_pase_attr_tree_clicked()
{
    if(m_CopyNode.type == CNONE)
    {
        return;
    }
    QTreeWidgetItem * curItem = ui->menu_tree->currentItem();
    Menu_Wnd *wnd = pinterface->getMenu_node(curItem);
    Menu_Wnd *parentWnd = wnd->parent;
    QString temp;
    QString suffix = wnd->node.toElement().attribute("Name");
    suffix.replace(wnd->node.toElement().attribute("ParentName"), "");
    qDebug() <<"suffix " +suffix;
    pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);

    do
    {
        parentWnd = parentWnd->rbroher;
        wnd = parentWnd->firstChild;
        temp = wnd->node.toElement().attribute("Name");
        temp.replace(wnd->node.toElement().attribute("ParentName"), "");
        if(temp == suffix)
        {
            qDebug() << "temp: "+temp;
            pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
        }

       do
        {
            wnd = wnd->rbroher;
            temp = wnd->node.toElement().attribute("Name");
            temp.replace(wnd->node.toElement().attribute("ParentName"), "");

            if(temp == suffix)
            {
                qDebug() << "temp: "+temp;
                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                break;
            }

        }while(wnd->rbroher != wnd);

    }while(parentWnd->rbroher != parentWnd);


    /*
   QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
   QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
   Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
   foreach (QTreeWidgetItem* item, itemlist) {
       foreach(Menu_Wnd * wnd, pinterface->menuWndList)
       {
           if(wnd->item == item)
           {
             //  cloneTree(wnd, copyTree);
           }
       }
   }*/
    showWndAttr(wnd);
    showtips("粘贴成功");
}

void xmlExample::on_actionAbout_triggered()
{
    QString about = tr(VERSION) +"\nBase on Qt 5.6.3 MinGW 32bit \n"
                             +"Author:wuyubo @2018.08.06 \n\n"
                             +"This a SkinTool Assistant!\n\n"
                             +"Github:https://github.com/wuyubo/skinAssistant\n"
                             +"Copyright © 2018 CVTE.All rights reserved.";
    QMessageBox::about(this, tr("About"), about);
}

void xmlExample::on_cb_Frame_currentIndexChanged(const QString &arg1)
{
    oldFrame = pinterface->getCurframe();
    pinterface->setCurframe(arg1);
    showWndlist();
}

void xmlExample::on_ptn_conver_clicked()
{
    if(opt_mode == MODE_SIMPLE)
    {
        set_mode(MODE_UI);
    }
    else
    {
        set_mode(MODE_SIMPLE);
    }
}

void xmlExample::set_mode(OPTION_MODE mode)
{
    opt_mode = mode;
}


void xmlExample::on_ptn_refresh_clicked()
{
    if(opt_mode != MODE_UI)
    {
        return;
    }
}


void xmlExample::on_ptn_alignleft_clicked()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.isText = false;
    m_CopyNode.isAdvanced = false;
    m_CopyNode.Align = ALIGN_LEFT;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_aligncenter_clicked()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_CENTER;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_alignright_clicked()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_RIGHT;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode, m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_alignup_clicked()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_UP;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_aligncenter_UD_clicked()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_CENTER_UD;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_aligndown_clicked()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_DOWN;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_moveleft_pos_clicked()
{
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    bool ok;
    int step = ui->le_step->text().toInt(&ok);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_LEFT;
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {
                copyWndAttr(wnd);
                m_CopyNode.position.X -= step;
                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::clearCopy()
{
    m_CopyNode.type = CNONE;
    m_CopyNode.isDisplay = false;
    m_CopyNode.isText = false;
    m_CopyNode.isAdvanced = false;
    m_CopyNode.Align = ALIGN_NORMAL;
}

void xmlExample::on_ptn_moveright_pos_clicked()
{
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    bool ok;
    int step = ui->le_step->text().toInt(&ok);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_LEFT;
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {
                copyWndAttr(wnd);
                m_CopyNode.position.X += step;
                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_moveup_pos_clicked()
{
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    bool ok;
    int step = ui->le_step->text().toInt(&ok);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_UP;
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {
                copyWndAttr(wnd);
                m_CopyNode.position.Y -= step;
                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_ptn_movedown_pos_clicked()
{
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    bool ok;
    int step = ui->le_step->text().toInt(&ok);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_DOWN;
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {
                copyWndAttr(wnd);
                m_CopyNode.position.Y += step;
                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                updateUI();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::copyWndAttr(Menu_Wnd *wnd)
{
    QDomNode n=wnd->node.firstChild();
    QDomNode pn = wnd->parent->node;
    bool ok;

    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position" && (m_CopyNode.type == CALL || m_CopyNode.isDisplay))
           {
                QDomElement pe = pinterface->getChildNode(pn.toElement(), e.tagName ());
                m_CopyNode.position.X = e.toElement().attribute("X").toInt(&ok);
                m_CopyNode.position.Y = e.toElement().attribute("Y").toInt(&ok);
                m_CopyNode.position.Px = pe.toElement().attribute("X").toInt(&ok);
                m_CopyNode.position.Py = pe.toElement().attribute("Y").toInt(&ok);
                m_CopyNode.position.Width = e.toElement().attribute("Width").toInt(&ok);
                m_CopyNode.position.Height = e.toElement().attribute("Height").toInt(&ok);
           }
           if(e.tagName () == "Text"&&(m_CopyNode.type == CALL || m_CopyNode.isText))
           {
                m_CopyNode.text.TextAlign = e.toElement().attribute("TextAlign");
                m_CopyNode.text.NormalTextColor = e.toElement().attribute("NormalTextColor");
                m_CopyNode.text.FocusTextColor = e.toElement().attribute("FocusTextColor");
                m_CopyNode.text.DisableTextColor = e.toElement().attribute("DisableTextColor");
                m_CopyNode.text.FontTypeNormal = e.toElement().attribute("FontTypeIndex");
                m_CopyNode.text.FontTypeFocus = e.toElement().attribute("FocusFontTypeIndex");
                m_CopyNode.text.FontTypeDisbale = e.toElement().attribute("DisableFontTypeIndex");

           }
           if(e.tagName () == "StaticWndProperties" && (m_CopyNode.type == CALL || m_CopyNode.isAdvanced))
           {
                m_CopyNode.properties.Clone = e.toElement().attribute("Clone");

                m_CopyNode.properties.FocusBgColor = e.toElement().attribute("FocusBgColor");
                m_CopyNode.properties.DisabledBgColor = e.toElement().attribute("DisabledBgColor");
                m_CopyNode.properties.NormalBgColor = e.toElement().attribute("NormalBgColor");

                m_CopyNode.properties.NormalBitmapID = e.toElement().attribute("NormalBitmapID");
                m_CopyNode.properties.FocusBitmapID = e.toElement().attribute("FocusBitmapID");
                m_CopyNode.properties.DisabledBitmapID = e.toElement().attribute("DisabledBitmapID");
                m_CopyNode.properties.BgState = e.toElement().attribute("BgState");

                m_CopyNode.properties.HasNormalDrawStyle = e.toElement().attribute("HasNormalDrawStyle");
                m_CopyNode.properties.HasFocusDrawStyle = e.toElement().attribute("HasFocusDrawStyle");
                m_CopyNode.properties.HasDisableDrawStyle = e.toElement().attribute("HasDisableDrawStyle");
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
    }
}


void  xmlExample::setWndAttr(Menu_Wnd *wnd)
{
    QDomNode n = wnd->node.firstChild();
    wnd->node.toElement().setAttribute("Name", ui->le_name->text());
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position")
           {
                e.toElement().setAttribute("X", ui->le_XPos->text());
                e.toElement().setAttribute("Y",ui->le_YPos->text());
                e.toElement().setAttribute("Width",ui->le_Width->text());
                e.toElement().setAttribute("Height", ui->le_Height->text());
           }
           if(e.tagName () == "Text")
           {
                e.toElement().setAttribute("TextAlign",ui->le_Align->text());
                e.toElement().setAttribute("TextID",ui->le_text->text());
                e.toElement().setAttribute("NormalTextColor", ui->le_NormColor->text());
                e.toElement().setAttribute("FocusTextColor", ui->le_FocColor->text());
                e.toElement().setAttribute("DisableTextColor",ui->le_disColor->text());
                e.toElement().setAttribute("FontTypeIndex",ui->le_font_size_0->text());
                e.toElement().setAttribute("FocusFontTypeIndex",ui->le_font_size_1->text());
                e.toElement().setAttribute("DisableFontTypeIndex",ui->le_font_size_2->text());
           }
           if(e.tagName () == "StaticWndProperties")
           {
                e.toElement().setAttribute("Clone", ui->le_clone->text());

                e.toElement().setAttribute("NormalBgColor", ui->le_anomcolor->text());
                e.toElement().setAttribute("FocusBgColor", ui->le_afoccolor->text());
                e.toElement().setAttribute("DisabledBgColor", ui->le_adiscolor->text());
                e.toElement().setAttribute("NormalBitmapID", ui->le_nomBitmap->text());
                e.toElement().setAttribute("FocusBitmapID", ui->le_focBitmap->text());
                e.toElement().setAttribute("DisabledBitmapID", ui->le_disBitmap->text());

                if(ui->rbn_clone->isChecked())
                {
                    e.toElement().setAttribute("BgState", "0");
                }else if(ui->rbn_color->isChecked())
                {
                    e.toElement().setAttribute("BgState", "1");
                }
                else if(ui->rbn_bitmap->isChecked())
                {
                    e.toElement().setAttribute("BgState", "2");
                }

                if(ui->cb_bg_nomal->isChecked())
                {
                    e.toElement().setAttribute("HasNormalDrawStyle", "1");
                }else
                {
                    e.toElement().setAttribute("HasNormalDrawStyle", "0");
                }
                if(ui->cb_bg_focus->isChecked())
                {
                    e.toElement().setAttribute("HasFocusDrawStyle", "1");
                }else
                {
                    e.toElement().setAttribute("HasFocusDrawStyle", "0");
                }
                if(ui->cb_bg_disble->isChecked())
                {
                    e.toElement().setAttribute("HasDisableDrawStyle", "1");
                }else
                {
                    e.toElement().setAttribute("HasDisableDrawStyle", "0");
                }
           }
           if(e.tagName () == "Navigation")
           {
                e.toElement().setAttribute("Up", ui->le_Navigation_up->text());
                e.toElement().setAttribute("Down", ui->le_Navigation_down->text());
                e.toElement().setAttribute("Left", ui->le_Navigation_left->text());
                e.toElement().setAttribute("Right", ui->le_Navigation_right->text());
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}

//================= main bar action  start========================================
void xmlExample::on_action_triggered()
{
    QString path =  pinterface->pSetting->getRootPath();
    path = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("Choose xml"), path, tr("*.xml"));
    if(pinterface->load_xml(path))
    {
        ui->cb_Frame->clear();
        foreach (QString frame, pinterface->frameList) {
            ui->cb_Frame->addItem(frame);
        }
        showWndlist();
        showtips("打开 "+path+" 成功");
    }
    else
    {
        showtips("打开 "+path+" 失败");
    }
}

void xmlExample::on_action_save_triggered()
{
    if(pinterface->saveXml())
    {
        showtips("保存成功");
    }else
    {
        showtips("保存失败");
    }
}

void xmlExample::updateUI()
{
    if(opt_mode == MODE_UI)
    {
        emit refreshUI_sig();
    }
}

void xmlExample::on_action_switch_triggered()
{
    pDialogUI->show();
    set_mode(MODE_UI);
    updateUI();
    //this->activateWindow();
}

void xmlExample::on_action_align_left_triggered()
{
    on_ptn_alignleft_clicked();
}

void xmlExample::on_action_alignright_triggered()
{
    on_ptn_alignright_clicked();
}

void xmlExample::on_action_alignCenter1_triggered()
{
    on_ptn_aligncenter_clicked();
}

void xmlExample::on_action_alignUp_triggered()
{
    on_ptn_alignup_clicked();
}

void xmlExample::on_action_alignCenter2_triggered()
{
    on_ptn_aligncenter_UD_clicked();
}

void xmlExample::on_action_alighDown_triggered()
{
    on_ptn_aligndown_clicked();
}
void xmlExample::on_action_postion_triggered()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_POSITION;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                emit refreshUI_sig();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

void xmlExample::on_action_size_triggered()
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    Menu_Wnd *curwnd = pinterface->getMenu_node(currentItem);
    clearCopy();
    m_CopyNode.type = CALIGN;
    m_CopyNode.isDisplay = true;
    m_CopyNode.Align = ALIGN_RESIZE;
    copyWndAttr(curwnd);
    ui->menu_tree->setFocus();
    foreach (QTreeWidgetItem* item, itemlist) {
        foreach(Menu_Wnd * wnd, pinterface->menuWndList)
        {
            if(wnd->item == item)
            {

                pinterface->cloneWndAttr(wnd, m_CopyNode,  m_CopyNode);
                emit refreshUI_sig();
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}

//================= main bar action  end========================================







void xmlExample::on_action_clean_triggered()
{
    pDialogClean->show();
}

void xmlExample::ItemShowHide(QTreeWidgetItem * item, bool isShow)
{
    int count;
    Menu_Wnd *wnd = pinterface->getMenu_node(item);
    if(wnd == NULL)
    {
        return;
    }
    wnd->isShow = isShow;
    if(isShow)
    {
        item->setCheckState(0,Qt::Checked);
        count=item->childCount();
        if(count>0)
        {
            for(int i=0;i<count;i++)
            {
                //子节点也被选中
                ItemShowHide(item->child(i), true);
            }
        }
    }else
    {
         item->setCheckState(0,Qt::Unchecked);
         count=item->childCount();
         if(count>0)
         {
             for(int i=0;i<count;i++)
             {
                 //子节点也被选中
                 ItemShowHide(item->child(i), false);
             }
         }
    }
}

void xmlExample::on_ptn_show_hide_clicked()
{
    QTreeWidgetItem * item = ui->menu_tree->currentItem();
    if(Qt::Checked==item->checkState(0))
    {
        ItemShowHide(item, false);
    }else
    {
        ItemShowHide(item, true);
    }
    emit refreshUI_sig();

}

void xmlExample::on_imgManager_triggered()
{
    pDialgImgManager->show();
    pDialgImgManager->showImgeList();
}
