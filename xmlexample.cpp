#include "xmlexample.h"
#include "ui_xmlexample.h"
#include <QFileDialog>

xmlExample::xmlExample(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::xmlExample)
{
  ui->setupUi(this);
  xmlReader = NULL;
  initCopyNode();

  connect(ui->menu_tree,SIGNAL(itemChanged(QTreeWidgetItem *,int)),this,SLOT(treeItemChanged(QTreeWidgetItem*,int)));
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
}

void xmlExample::on_ptn_read_clicked()
{
    filepath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("Choose xml"),
                                                    "./",tr("*.xml"));
    if(filepath != NULL)
    {
        copyFileToPath(filepath, BACK_UP_FILE, true);
        copyFileToPath(filepath, WORK_FILE, true);

        QFile file(WORK_FILE);

        if (!file.open(QFile::ReadOnly)) {
          qDebug() << "Cannot read file";
          return;
        }

        m_docElem = xmlContents.xmlRead(&file);

        file.close();
        initTreeWidget();
        showtips("打开"+filepath);
    }
}

void xmlExample::on_ptn_write_clicked()
{
    if(filepath != NULL)
    {
        QFile file(WORK_FILE);

        if (!file.open(QFile::WriteOnly)) {
          qDebug() << "Cannot open file";
          return;
        }

        xmlContents.changeSave(&file);
        file.close();
        qDebug() << filepath;
        copyFileToPath(WORK_FILE, filepath, true);
        showtips("保存"+filepath);
    }
}

void xmlExample::showWndlist()
{
    QTreeWidgetItem *group1;
    ui->menu_tree->clear();
    foreach(Menu_Wnd * wnd, menuWndList)
    {
        if(wnd->frame != ui->cb_Frame->currentText())
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

Menu_Wnd *xmlExample::getParentWnd(QString parenName, Menu_Wnd *prevWnd)
{
    if(parenName == "")
    {
        return NULL;
    }
    else if(parenName == prevWnd->node.toElement().attribute("Name"))
    {
        return prevWnd;
    }
    else
    {
        return getParentWnd(parenName, prevWnd->parent);
    }
}

void xmlExample::GetWndList(QString frame, QDomNode node)
{
    QDomNodeList list = node.toElement().childNodes();
    Menu_Wnd * wnd, *wnd2;
    QString parentName;

    for (int i = 0; i< list.count();i++)
    {
         QDomNode node2 = list.at(i);
         if (node2.isElement())
         {
            if(node2.toElement().tagName() == "Wnd")
            {
              //  qDebug() << node2.toElement().attribute("Name");
                parentName = node2.toElement().attribute("ParentName");
                if(node2.toElement().attribute("CtrlTypeName") == "Main Frame")
                {
                    wnd = new Menu_Wnd();
                    wnd->node = node2;
                    wnd->parent = wnd;
                    wnd->frame = frame;
                    wnd->isMainFrame = true;
                    menuWndList.append(wnd);
                }else
                {
                    wnd2 = new Menu_Wnd();
                    wnd2->node = node2;
                    wnd2->parent = getParentWnd(parentName, wnd);
                    wnd2->isMainFrame = false;
                    wnd2->frame = frame;
                    menuWndList.append(wnd2);
                    wnd = wnd2;
                }
            }
         }
 }
}

void xmlExample::GetGWnd(QDomNode node)
{
     QDomNodeList list = node.toElement().childNodes();
     ui->cb_Frame->addItem(node.toElement().attribute("Name"));
     for (int i = 0; i< list.count ();i++)
     {
         QDomNode node2 = list.at(i);
         if (node2.isElement())
         {
            if(node2.toElement().tagName() == "WndList")
            {
               GetWndList(node.toElement().attribute("Name"), node2);
            }
         }
     }

}

//initTreeWidget() 的实现
void xmlExample::initTreeWidget()
{
    ui->menu_tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
   // listWidget.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection) #设置选择模式

   // 选择模式有:ExtendedSelection 按住ctrl多选, SingleSelection 单选 MultiSelection 点击多选 ContiguousSelection 鼠标拖拉多选
     QDomNode n=m_docElem.firstChild();

     while (!n.isNull ())
     {
        if (n.isElement ()) {
            //将其转换为元素
            QDomElement e = n.toElement ();

            if(e.tagName () == "GWndList")
            {
                QDomNodeList list = e.childNodes ();
                for (int i = 0; i< list.count ();i++)
                {
                    QDomNode node = list.at(i);

                    if (node.isElement ())
                    {
                        if(node.toElement().tagName() == "GWnd")
                        {
                            GetGWnd(node);
                        }
                        //qDebug() << qPrintable(node.toElement().tagName ()) << qPrintable(node.toElement().attribute("Name"));
                    }
                }
            }
        }
        n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
    }
    showWndlist();
}

//treeItemChanged(QTreeWidgetItem *item, int column)的实现
void xmlExample::treeItemChanged(QTreeWidgetItem *item, int column)
{
    QString itemText=item->text(0);
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
    }
}

//updateParentItem(QTreeWidgetItem *item) 的实现
void xmlExample::updateParentItem(QTreeWidgetItem *item)
{
  /*  QTreeWidgetItem* parent=item->parent();
    if(parent==NULL)
    {
        return;
    }
    //选中的子节点个数
    int selectCount=0;
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

     Menu_Wnd *wnd = getMenu_node(currentItem);
     if(wnd)
     {
         setWndAttr(wnd);
         currentItem->setText(0, ui->le_name->text());
     }
}

Menu_Wnd * xmlExample::getMenu_node(QTreeWidgetItem * item)
{
    foreach(Menu_Wnd * wnd, menuWndList)
    {
        if(wnd->item == item)
        {
            return wnd;
        }
    }
    return NULL;
}


void xmlExample::on_cb_Frame_currentIndexChanged(int index)
{
    index = index;
    showWndlist();
}

void xmlExample::on_menu_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    Menu_Wnd *wnd = getMenu_node(item);
    column = column;
    if(wnd)
    {
        showWndAttr(wnd);
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
                e.toElement().setAttribute("NormalTextColor", ui->le_NormColor->text());
                e.toElement().setAttribute("FocusTextColor", ui->le_FocColor->text());
                e.toElement().setAttribute("DisableTextColor",ui->le_disColor->text());
           }
           if(e.tagName () == "StaticWndProperties")
           {
                e.toElement().setAttribute("Clone", ui->le_clone->text());
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}

void  xmlExample::cloneWndAttr(Menu_Wnd *wnd, CNode cnode)
{
    QDomNode n = wnd->node.firstChild();

    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position" && (m_CopyNode.type == CALL || m_CopyNode.isDisplay))
           {
                e.toElement().setAttribute("X", cnode.position.X);
                e.toElement().setAttribute("Y", cnode.position.Y);
                e.toElement().setAttribute("Width",cnode.position.Width);
                e.toElement().setAttribute("Height", cnode.position.Height);
           }
           if(e.tagName () == "Text"&& (m_CopyNode.type == CALL || m_CopyNode.isText))
           {
                e.toElement().setAttribute("TextAlign",cnode.text.TextAlign);
                e.toElement().setAttribute("NormalTextColor", cnode.text.NormalTextColor);
                e.toElement().setAttribute("FocusTextColor", cnode.text.FocusTextColor);
                e.toElement().setAttribute("DisableTextColor",cnode.text.DisableTextColor);
           }
           if(e.tagName () == "StaticWndProperties" && (m_CopyNode.type == CALL || m_CopyNode.isAdvanced))
           {
                e.toElement().setAttribute("Clone", cnode.properties.Clone);
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}

void xmlExample::showWndAttr(Menu_Wnd *wnd)
{
    QDomNode n=wnd->node.firstChild();

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
           }
           if(e.tagName () == "Text")
           {
                ui->le_Align->setText(e.toElement().attribute("TextAlign"));
                ui->le_NormColor->setText(e.toElement().attribute("NormalTextColor"));
                ui->le_FocColor->setText(e.toElement().attribute("FocusTextColor"));
                ui->le_disColor->setText(e.toElement().attribute("DisableTextColor"));
           }
           if(e.tagName () == "StaticWndProperties")
           {
                ui->le_clone->setText(e.toElement().attribute("Clone"));
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}

void xmlExample::copyWndAttr(Menu_Wnd *wnd)
{
    QDomNode n=wnd->node.firstChild();

    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position" && (m_CopyNode.type == CALL || m_CopyNode.isDisplay))
           {
                m_CopyNode.position.X = e.toElement().attribute("X");
                m_CopyNode.position.Y = e.toElement().attribute("Y");
                m_CopyNode.position.Width = e.toElement().attribute("Width");
                m_CopyNode.position.Height = e.toElement().attribute("Height");
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

    Menu_Wnd *wnd = getMenu_node(currentItem);
    if(wnd)
    {
        initCopyNode();
        if(ui->cb_all->isChecked())
        {
           m_CopyNode.type = CALL;
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
        copyWndAttr(wnd);
    }
}

void xmlExample::on_ptn_pase_attr_clicked()
{
    if(m_CopyNode.type == CNONE)
    {
        return;
    }
   QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
   QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
   Menu_Wnd *curwnd = getMenu_node(currentItem);
   foreach (QTreeWidgetItem* item, itemlist) {
       foreach(Menu_Wnd * wnd, menuWndList)
       {
           if(wnd->item == item)
           {
               cloneWndAttr(wnd, m_CopyNode);
           }
       }
   }
    showWndAttr(curwnd);
}

void xmlExample::add_xmlnode(CopyNode cnode)
{
    QTreeWidgetItem * currentItem = ui->menu_tree->currentItem();
    Menu_Wnd *curwnd = getMenu_node(currentItem);
    QDomElement newNode = xmlContents.xmlCreateNode(cnode.node.toElement().tagName());
   // QDomAttr attr_id = doc.createAttribute(tr("id"));
    //curwnd->node.insertBefore();
    curwnd->node.appendChild(newNode);
}

void xmlExample::showtips(QString tips)
{
    ui->lb_tips->setText(tips);
}


bool xmlExample::copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}
