#include "xmlexample.h"
#include "ui_xmlexample.h"
#include "xlabel.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QAbstractItemView>

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
  pDialogUI  = new DialogUI(pinterface, this);
  pDialogImageClone = new DialogImageClone(pinterface, this);
  pDialogClean = new DialogClean(pinterface, this);
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
    /*
    Menu_Wnd *wnd = getMenu_node(item);
    column = column;
    if(item->checkState(0)==Qt::Checked)
    {
        showTreetoUI(wnd);
    }else
    {
        hideWndFromUi(wnd);
    }
    */
   /* QString itemText=item->text(0);
    column = column;
    //选中时
    if(Qt::Checked==item->checkState(0))
    {
        QTreeWidgetItem* parent=item->parent();
        int count=item->childCount();
        if(count>0)
        {
            for(int i=0;i<count;i++)
            {
                //子节点也被选中
                item->child(i)->setCheckState(0,Qt::Checked);
            }
        }
        else
        {
            //是子节点
            updateParentItem(item);
        }
    }
    else if(Qt::Unchecked==item->checkState(0))
    {
        int count=item->childCount();
        if(count>0)
        {
            for(int i=0;i<count;i++)
            {
                item->child(i)->setCheckState(0,Qt::Unchecked);
            }
        }
        else
        {
            //是子节点
            updateParentItem(item);
        }
    }*/
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
     if(opt_mode == MODE_UI)
     {
         refreshWndFromUi(wnd);
     }
}


void xmlExample::on_menu_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    Menu_Wnd *wnd = pinterface->getMenu_node(item);
    column = column;
    if(wnd)
    {
        showWndAttr(wnd);
        if(opt_mode == MODE_UI)
        {
            refreshWndFromUi(wnd);
        }
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
                if(ui->le_text->text() != "")
                {
                    ui->lb_img->setText(pinterface->getString(ui->le_text->text(), "English"));
                }
           }
           if(e.tagName () == "StaticWndProperties")
           {
                QString state = e.toElement().attribute("BgState");
                QString color_Nomal = e.toElement().attribute("HasNormalText");
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
                }
                if(color_Focus == "1")
                {
                    ui->cb_bg_focus->setChecked(true);
                }
                if(color_Disble == "1")
                {
                    ui->cb_bg_disble->setChecked(true);
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
               if(opt_mode == MODE_UI)
               {
                   refreshWndFromUi(wnd);
               }
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
    switch (mode) {
    case MODE_SIMPLE:
       // ui->groupBox_img->show();
        ui->lb_mask->show();
        //ui->groupBox_align->hide();
        ui->groupBox_img->hide();
      //  ui->groupBox_attr->setGeometry(100,200,491, 791);
      // ui->groupBox_tree->setGeometry(610,200,691, 791);
        ui->ptn_refresh->hide();
        break;
    case MODE_UI:
        ui->groupBox_img->hide();
        ui->lb_mask->hide();
        //ui->groupBox_align->show();
        ui->groupBox_attr->setGeometry(1380,200,491, 791);
        ui->groupBox_tree->setGeometry(1880,200,591, 791);
        ui->ptn_refresh->show();
        break;
    default:
        break;
    }
}

void xmlExample::showCloneStaticWndProperties(Menu_Wnd *show_wnd, Menu_Wnd *wnd)
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

void xmlExample::showWndToUi(Menu_Wnd *wnd)
{
    if(opt_mode != MODE_UI)
    {
        return;
    }
    QDomNode n = wnd->node.firstChild();
    bool ok;
    qDebug() << "-----------------------------";
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
                    qDebug() << "-----------------------------"<< pinterface->getString(e.toElement().attribute("TextID"), "English");
                    wnd->label->setText(pinterface->getString(e.toElement().attribute("TextID"), "English"));
                    //qDebug() << getString(e.toElement().attribute("TextID"), "English");
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

                }else if(state == "1")
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

                }
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

void xmlExample::hideWndFromUi(Menu_Wnd *wnd)
{
    if(opt_mode != MODE_UI)
    {
        return;
    }
     wnd->label->hide();
     wnd->isShow = false;
}

void xmlExample::refreshWndFromUi(Menu_Wnd *wnd)
{
    if(opt_mode != MODE_UI)
    {
        return;
    }
    //qDebug() << wnd->node.toElement().attribute("Name")<< wnd->item->checkState(0);
    if(Qt::Checked == wnd->item->checkState(0))
    {
        showWndToUi(wnd);
    }
    else
    {
        hideWndFromUi(wnd);
    }
}


void xmlExample::showTreetoUI(Menu_Wnd *w)
{
    if(opt_mode != MODE_UI)
    {
        return;
    }
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
void xmlExample::hideTreeFromUI(Menu_Wnd *w)
{
    if(opt_mode != MODE_UI)
    {
        return;
    }
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

void xmlExample::on_ptn_refresh_clicked()
{
    if(opt_mode != MODE_UI)
    {
        return;
    }
    pinterface->setCurframe(ui->cb_Frame->currentText());
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
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

void xmlExample::on_action_switch_triggered()
{

    pDialogUI->show();
    emit refreshUI_sig();
   // pDialogUI->refreshUI();

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
                refreshWndFromUi(wnd);
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
                refreshWndFromUi(wnd);
                ui->menu_tree->setItemSelected(item,true);
                showWndAttr(wnd);
            }
        }
    }
}
void xmlExample::on_action_imageClone_triggered()
{
    pDialogImageClone->show();
}
//================= main bar action  end========================================







void xmlExample::on_action_clean_triggered()
{
    pDialogClean->show();
}

void xmlExample::on_ptn_show_hide_clicked()
{
    QTreeWidgetItem * item = ui->menu_tree->currentItem();
    if(Qt::Checked==item->checkState(0))
    {
        item->setCheckState(0,Qt::Unchecked);
    }
    else if(Qt::Unchecked==item->checkState(0))
    {
        item->setCheckState(0,Qt::Checked);
    }
    //选中时
    if(Qt::Checked==item->checkState(0))
    {
       // QTreeWidgetItem* parent=item->parent();
        int count=item->childCount();
        if(count>0)
        {
            for(int i=0;i<count;i++)
            {
                //子节点也被选中
                item->child(i)->setCheckState(0,Qt::Checked);
            }
        }
        else
        {
            //是子节点
            updateParentItem(item);
        }
    }
    else if(Qt::Unchecked==item->checkState(0))
    {
        int count=item->childCount();
        if(count>0)
        {
            for(int i=0;i<count;i++)
            {
                item->child(i)->setCheckState(0,Qt::Unchecked);
            }
        }
        else
        {
            //是子节点
            updateParentItem(item);
        }
    }
}
