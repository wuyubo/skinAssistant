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
    m_CopyNode.Name = "";
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

        if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
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
Menu_Wnd * xmlExample::getlbrother(Menu_Wnd *curwnd, Menu_Wnd *prewnd)
{
    if(curwnd->parent == prewnd)
    {
      return curwnd;
    }
    else if(curwnd->parent == prewnd->parent)
    {
        return prewnd;
    }
    return getlbrother(curwnd, prewnd->parent);
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
                    wnd->lbroher = wnd;
                    wnd->rbroher = wnd;
                    wnd->frame = frame;
                    wnd->firstChild = wnd;
                    wnd->isMainFrame = true;
                    menuWndList.append(wnd);
                }else
                {
                    wnd2 = new Menu_Wnd();
                    wnd2->node = node2;
                    wnd2->parent = getParentWnd(parentName, wnd);
                    wnd2->isMainFrame = false;
                    wnd2->frame = frame;
                    wnd2->lbroher = getlbrother(wnd2, wnd);
                    if(wnd2->parent == wnd)
                    {
                        wnd->firstChild = wnd2;
                    }
                    if(wnd2->parent == wnd->parent)
                    {
                        wnd->rbroher = wnd2;
                    }
                    wnd2->rbroher = wnd2;
                    wnd2->firstChild = wnd2;
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
               if(cnode.position.X != "")
               {
                    e.toElement().setAttribute("X", cnode.position.X);
               }
               if(cnode.position.Y != "")
               {
                    e.toElement().setAttribute("Y", cnode.position.Y);
               }
               if(cnode.position.Height != "")
               {
                    e.toElement().setAttribute("Width",cnode.position.Width);
               }
               if(cnode.position.Height != "")
               {
                    e.toElement().setAttribute("Height", cnode.position.Height);
               }
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
                 m_CopyNode.position.X = "";
            }
            if(ui->le_YPos->text() == "")
            {
                 m_CopyNode.position.Y = "";
            }
            if(ui->le_Width->text() == "")
            {
                 m_CopyNode.position.Width = "";
            }
            if(ui->le_Height->text() == "")
            {
                 m_CopyNode.position.Height = "";
            }

        }
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
QDomElement xmlExample::getChildNode(QDomElement node, QString childName)
{
    QDomNode n=node .firstChild();

    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == childName )
           {
               return e;
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
    }
    return node;
}

Menu_Wnd * xmlExample::add_xmlnode(Menu_Wnd *wnd, Menu_Wnd *copywnd, CopyNode *pcnode, ADD_NODE_TYPE type)
{
    QDomElement cureNode = copywnd->node.toElement();
    QDomElement insertNode = wnd->node.toElement();
    QDomElement newNode = xmlContents.xmlCreateNode("Wnd");
    QDomElement childNode, tempNode;
    QDomAttr attr_CtrlTypeName = xmlContents.xmlCreateAttribute(tr("CtrlTypeName"));
    QDomAttr attr_Name = xmlContents.xmlCreateAttribute(tr("Name"));
    QDomAttr attr_ParentName = xmlContents.xmlCreateAttribute(tr("ParentName"));
    QDomAttr attr_Lock = xmlContents.xmlCreateAttribute(tr("Lock"));
    attr_CtrlTypeName.setValue(cureNode.attribute("CtrlTypeName"));

    if(pcnode == NULL || pcnode->Name == "")
    {
        attr_Name.setValue(cureNode.attribute("Name"));
    }else
    {
        attr_Name.setValue(wnd->parent->node.toElement().attribute("Name")+pcnode->Name);
    }

    attr_ParentName.setValue(cureNode.attribute("ParentName"));
    attr_Lock.setValue(cureNode.attribute("Lock"));
    newNode.setAttributeNode(attr_CtrlTypeName);
    newNode.setAttributeNode(attr_Name);
    newNode.setAttributeNode(attr_ParentName);
    newNode.setAttributeNode(attr_Lock);
//----------------------
    QDomElement element_Position = xmlContents.xmlCreateNode("Position");

    QDomAttr attr_X = xmlContents.xmlCreateAttribute(tr("X"));
    QDomAttr attr_Y = xmlContents.xmlCreateAttribute(tr("Y"));
    QDomAttr attr_Width = xmlContents.xmlCreateAttribute(tr("Width"));
    QDomAttr attr_Height = xmlContents.xmlCreateAttribute(tr("Height"));

    if(pcnode != NULL && pcnode->isDisplay)
    {
        childNode = getChildNode(pcnode->node.toElement(), "Position");
        tempNode = getChildNode(cureNode, "Position");
    }else
    {
        childNode = getChildNode(cureNode, "Position");
    }

    attr_X.setValue(childNode.attribute("X"));
    if(pcnode != NULL && pcnode->isDisplay)
    {
        attr_Y.setValue(tempNode.attribute("Y"));
    }else
    {
        attr_Y.setValue(childNode.attribute("Y"));
    }

    attr_Width.setValue(childNode.attribute("Width"));
    attr_Height.setValue(childNode.attribute("Height"));

    element_Position.setAttributeNode(attr_X);
    element_Position.setAttributeNode(attr_Y);
    element_Position.setAttributeNode(attr_Width);
    element_Position.setAttributeNode(attr_Height);
//-----------------
    QDomElement element_State = xmlContents.xmlCreateNode("State");

    QDomAttr attr_Focusable = xmlContents.xmlCreateAttribute(tr("Focusable"));
    QDomAttr attr_Disabled = xmlContents.xmlCreateAttribute(tr("Disabled"));
    QDomAttr attr_Visible = xmlContents.xmlCreateAttribute(tr("Visible"));
    QDomAttr attr_SrcAlphaReplaceDstAlpha = xmlContents.xmlCreateAttribute(tr("SrcAlphaReplaceDstAlpha"));

    childNode = getChildNode(cureNode, "State");

    attr_Focusable.setValue(childNode.attribute("Focusable"));
    attr_Disabled.setValue(childNode.attribute("Disabled"));
    attr_Visible.setValue(childNode.attribute("Visible"));
    attr_SrcAlphaReplaceDstAlpha.setValue(childNode.attribute("SrcAlphaReplaceDstAlpha"));

    element_State.setAttributeNode(attr_Focusable);
    element_State.setAttributeNode(attr_Disabled);
    element_State.setAttributeNode(attr_Visible);
    element_State.setAttributeNode(attr_SrcAlphaReplaceDstAlpha);
//-------------------
    QDomElement element_Text = xmlContents.xmlCreateNode("Text");

    QDomAttr attr_HasFocusText = xmlContents.xmlCreateAttribute(tr("HasFocusText"));
    QDomAttr attr_TextDistance = xmlContents.xmlCreateAttribute(tr("TextDistance"));
    QDomAttr attr_FocusTextColor = xmlContents.xmlCreateAttribute(tr("FocusTextColor"));
    QDomAttr attr_FocusFontTypeIndex = xmlContents.xmlCreateAttribute(tr("FocusFontTypeIndex"));
    QDomAttr attr_DisableTextColor = xmlContents.xmlCreateAttribute(tr("DisableTextColor"));
    QDomAttr attr_HasNormalText = xmlContents.xmlCreateAttribute(tr("HasNormalText"));
    QDomAttr attr_TextID = xmlContents.xmlCreateAttribute(tr("TextID"));
    QDomAttr attr_NormalTextColor = xmlContents.xmlCreateAttribute(tr("NormalTextColor"));
    QDomAttr attr_FontTypeIndex = xmlContents.xmlCreateAttribute(tr("FontTypeIndex"));
    QDomAttr attr_HasDisableText = xmlContents.xmlCreateAttribute(tr("HasDisableText"));
    QDomAttr attr_TextAlign = xmlContents.xmlCreateAttribute(tr("TextAlign"));
    QDomAttr attr_DisableFontTypeIndex = xmlContents.xmlCreateAttribute(tr("DisableFontTypeIndex"));

    if(pcnode != NULL && pcnode->isText)
    {
        childNode = getChildNode(pcnode->node.toElement(), "Text");
    }else
    {
        childNode = getChildNode(cureNode, "Text");
    }

    attr_HasFocusText.setValue(childNode.attribute("HasFocusText"));
    attr_TextDistance.setValue(childNode.attribute("TextDistance"));
    attr_FocusTextColor.setValue(childNode.attribute("FocusTextColor"));
    attr_FocusFontTypeIndex.setValue(childNode.attribute("FocusFontTypeIndex"));
    attr_DisableTextColor.setValue(childNode.attribute("DisableTextColor"));
    attr_HasNormalText.setValue(childNode.attribute("HasNormalText"));
    attr_TextID.setValue(childNode.attribute("TextID"));
    attr_NormalTextColor.setValue(childNode.attribute("NormalTextColor"));
    attr_FontTypeIndex.setValue(childNode.attribute("FontTypeIndex"));
    attr_HasDisableText.setValue(childNode.attribute("HasDisableText"));
    attr_TextAlign.setValue(childNode.attribute("TextAlign"));
    attr_DisableFontTypeIndex.setValue(childNode.attribute("DisableFontTypeIndex"));

    element_Text.setAttributeNode(attr_HasFocusText);
    element_Text.setAttributeNode(attr_TextDistance);
    element_Text.setAttributeNode(attr_FocusTextColor);
    element_Text.setAttributeNode(attr_FocusFontTypeIndex);
    element_Text.setAttributeNode(attr_DisableTextColor);
    element_Text.setAttributeNode(attr_HasNormalText);
    element_Text.setAttributeNode(attr_TextID);
    element_Text.setAttributeNode(attr_NormalTextColor);
    element_Text.setAttributeNode(attr_FontTypeIndex);
    element_Text.setAttributeNode(attr_HasDisableText);
    element_Text.setAttributeNode(attr_TextAlign);
    element_Text.setAttributeNode(attr_DisableFontTypeIndex);

///-------------------
    QDomElement element_Frame = xmlContents.xmlCreateNode("Frame");

    QDomAttr attr_NormalFrame = xmlContents.xmlCreateAttribute(tr("NormalFrame"));
    QDomAttr attr_FocusFrame = xmlContents.xmlCreateAttribute(tr("FocusFrame"));
    QDomAttr attr_DisableFrame = xmlContents.xmlCreateAttribute(tr("DisableFrame"));
    QDomAttr attr_NormalFrameTopLeftColor = xmlContents.xmlCreateAttribute(tr("NormalFrameTopLeftColor"));
    QDomAttr attr_NormalFrameBottomRightColor = xmlContents.xmlCreateAttribute(tr("NormalFrameBottomRightColor"));
    QDomAttr attr_FocusFrameTopLeftColor = xmlContents.xmlCreateAttribute(tr("FocusFrameTopLeftColor"));
    QDomAttr attr_FocusFrameBottomRightColor = xmlContents.xmlCreateAttribute(tr("FocusFrameBottomRightColor"));
    QDomAttr attr_DisableFrameTopLeftColor = xmlContents.xmlCreateAttribute(tr("DisableFrameTopLeftColor"));
    QDomAttr attr_DisableFrameBottomRightColor = xmlContents.xmlCreateAttribute(tr("DisableFrameBottomRightColor"));


    childNode = getChildNode(cureNode, "Frame");

    attr_NormalFrame.setValue(childNode.attribute("NormalFrame"));
    attr_FocusFrame.setValue(childNode.attribute("FocusFrame"));
    attr_DisableFrame.setValue(childNode.attribute("DisableFrame"));
    attr_NormalFrameTopLeftColor.setValue(childNode.attribute("NormalFrameTopLeftColor"));
    attr_NormalFrameBottomRightColor.setValue(childNode.attribute("NormalFrameBottomRightColor"));
    attr_FocusFrameTopLeftColor.setValue(childNode.attribute("FocusFrameTopLeftColor"));
    attr_FocusFrameBottomRightColor.setValue(childNode.attribute("FocusFrameBottomRightColor"));
    attr_DisableFrameTopLeftColor.setValue(childNode.attribute("DisableFrameTopLeftColor"));
    attr_DisableFrameBottomRightColor.setValue(childNode.attribute("DisableFrameBottomRightColor"));

    element_Frame.setAttributeNode(attr_NormalFrame);
    element_Frame.setAttributeNode(attr_FocusFrame);
    element_Frame.setAttributeNode(attr_DisableFrame);
    element_Frame.setAttributeNode(attr_NormalFrameTopLeftColor);
    element_Frame.setAttributeNode(attr_NormalFrameBottomRightColor);
    element_Frame.setAttributeNode(attr_FocusFrameTopLeftColor);
    element_Frame.setAttributeNode(attr_FocusFrameBottomRightColor);
    element_Frame.setAttributeNode(attr_DisableFrameTopLeftColor);
    element_Frame.setAttributeNode(attr_DisableFrameBottomRightColor);

//------------------
    QDomElement element_Border = xmlContents.xmlCreateNode("Border");

    QDomAttr attr_NormalBorderSize = xmlContents.xmlCreateAttribute(tr("NormalBorderSize"));
    QDomAttr attr_FocusBorderSize = xmlContents.xmlCreateAttribute(tr("FocusBorderSize"));
    QDomAttr attr_DisableBorderSize = xmlContents.xmlCreateAttribute(tr("DisableBorderSize"));

    childNode = getChildNode(cureNode, "Border");

    attr_NormalBorderSize.setValue(childNode.attribute("NormalBorderSize"));
    attr_FocusBorderSize.setValue(childNode.attribute("FocusBorderSize"));
    attr_DisableBorderSize.setValue(childNode.attribute("DisableBorderSize"));

    element_Border.setAttributeNode(attr_NormalBorderSize);
    element_Border.setAttributeNode(attr_FocusBorderSize);
    element_Border.setAttributeNode(attr_DisableBorderSize);
//----------
    QDomElement element_Alpha = xmlContents.xmlCreateNode("Alpha");

    QDomAttr attr_NormalAlpha = xmlContents.xmlCreateAttribute(tr("NormalAlpha"));
    QDomAttr attr_FocusAlpha = xmlContents.xmlCreateAttribute(tr("FocusAlpha"));
    QDomAttr attr_DisableAlpha = xmlContents.xmlCreateAttribute(tr("DisableAlpha"));

    childNode = getChildNode(cureNode, "Alpha");

    attr_NormalAlpha.setValue(childNode.attribute("NormalAlpha"));
    attr_FocusAlpha.setValue(childNode.attribute("FocusAlpha"));
    attr_DisableAlpha.setValue(childNode.attribute("DisableAlpha"));

    element_Alpha.setAttributeNode(attr_NormalAlpha);
    element_Alpha.setAttributeNode(attr_FocusAlpha);
    element_Alpha.setAttributeNode(attr_DisableAlpha);

//---------------------------
    QDomElement element_Effect = xmlContents.xmlCreateNode("Effect");

    QDomAttr attr_TransEffIn = xmlContents.xmlCreateAttribute(tr("TransEffIn"));
    QDomAttr attr_TransEffOut = xmlContents.xmlCreateAttribute(tr("TransEffOut"));

    childNode = getChildNode(cureNode, "Effect");

    attr_TransEffIn.setValue(childNode.attribute("TransEffIn"));
    attr_TransEffOut.setValue(childNode.attribute("TransEffOut"));

    element_Effect.setAttributeNode(attr_TransEffIn);
    element_Effect.setAttributeNode(attr_TransEffOut);
 //-------------------
    QDomElement element_Navigation = xmlContents.xmlCreateNode("Navigation");

    QDomAttr attr_Up = xmlContents.xmlCreateAttribute(tr("Up"));
    QDomAttr attr_Down = xmlContents.xmlCreateAttribute(tr("Down"));
    QDomAttr attr_Left = xmlContents.xmlCreateAttribute(tr("Left"));
    QDomAttr attr_Right = xmlContents.xmlCreateAttribute(tr("Right"));

    childNode = getChildNode(cureNode, "Navigation");

    attr_Up.setValue(newNode.attribute("Name"));
    attr_Down.setValue(newNode.attribute("Name"));
    attr_Left.setValue(newNode.attribute("Name"));
    attr_Right.setValue(newNode.attribute("Name"));

    element_Navigation.setAttributeNode(attr_Up);
    element_Navigation.setAttributeNode(attr_Down);
    element_Navigation.setAttributeNode(attr_Left);
    element_Navigation.setAttributeNode(attr_Right);

 //---------------
    QDomElement element_KeyeventList = xmlContents.xmlCreateNode("KeyeventList");

    QDomAttr attr_KeyeventNum = xmlContents.xmlCreateAttribute(tr("KeyeventNum"));

    childNode = getChildNode(cureNode, "KeyeventList");

    attr_KeyeventNum.setValue(childNode.attribute("KeyeventNum"));

    element_KeyeventList.setAttributeNode(attr_KeyeventNum);
//--------------------
    QDomElement element_StaticWndProperties = xmlContents.xmlCreateNode("StaticWndProperties");

    QDomAttr attr_Text = xmlContents.xmlCreateAttribute(tr("Text"));
    QDomAttr attr_TextColor = xmlContents.xmlCreateAttribute(tr("TextColor"));
    QDomAttr attr_NormalBitmapID = xmlContents.xmlCreateAttribute(tr("NormalBitmapID"));
    QDomAttr attr_FocusBitmapID = xmlContents.xmlCreateAttribute(tr("FocusBitmapID"));
    QDomAttr attr_DisabledBitmapID = xmlContents.xmlCreateAttribute(tr("DisabledBitmapID"));
    QDomAttr attr_NormalBgColor = xmlContents.xmlCreateAttribute(tr("NormalBgColor"));
    QDomAttr attr_FocusBgColor = xmlContents.xmlCreateAttribute(tr("FocusBgColor"));
    QDomAttr attr_DisabledBgColor = xmlContents.xmlCreateAttribute(tr("DisabledBgColor"));
    QDomAttr attr_BgState = xmlContents.xmlCreateAttribute(tr("BgState"));
    QDomAttr attr_Alpha = xmlContents.xmlCreateAttribute(tr("Alpha"));
    QDomAttr attr_FrameColorTop = xmlContents.xmlCreateAttribute(tr("FrameColorTop"));
    QDomAttr attr_FrameColorBottom = xmlContents.xmlCreateAttribute(tr("FrameColorBottom"));
    QDomAttr attr_Clone = xmlContents.xmlCreateAttribute(tr("Clone"));
    QDomAttr attr_HasNormalDrawStyle = xmlContents.xmlCreateAttribute(tr("HasNormalDrawStyle"));
    QDomAttr attr_HasFocusDrawStyle = xmlContents.xmlCreateAttribute(tr("HasFocusDrawStyle"));
    QDomAttr attr_HasDisableDrawStyle = xmlContents.xmlCreateAttribute(tr("HasDisableDrawStyle"));
    QDomAttr attr_HasNormalPaletteLock= xmlContents.xmlCreateAttribute(tr("HasNormalPaletteLock"));
    QDomAttr attr_HasFocusPaletteLock = xmlContents.xmlCreateAttribute(tr("HasFocusPaletteLock"));
    QDomAttr attr_HasDisablePaletteLock = xmlContents.xmlCreateAttribute(tr("HasDisablePaletteLock"));
    QDomAttr attr_NormalPaletteIndex = xmlContents.xmlCreateAttribute(tr("NormalPaletteIndex"));
    QDomAttr attr_FocusPaletteIndex = xmlContents.xmlCreateAttribute(tr("FocusPaletteIndex"));
    QDomAttr attr_DisablePaletteIndex = xmlContents.xmlCreateAttribute(tr("DisablePaletteIndex"));


    if(pcnode != NULL && pcnode->isAdvanced)
    {
        childNode = getChildNode(pcnode->node.toElement(), "StaticWndProperties");
    }else
    {
        childNode = getChildNode(cureNode, "StaticWndProperties");
    }

    attr_Text.setValue(childNode.attribute("Text"));
    attr_TextColor.setValue(childNode.attribute("TextColor"));
    attr_NormalBitmapID.setValue(childNode.attribute("NormalBitmapID"));
    attr_DisabledBitmapID.setValue(childNode.attribute("DisabledBitmapID"));

    attr_NormalBgColor.setValue(childNode.attribute("NormalBgColor"));
    attr_FocusBitmapID.setValue(childNode.attribute("FocusBitmapID"));
    attr_FocusBgColor.setValue(childNode.attribute("FocusBgColor"));
    attr_DisabledBgColor.setValue(childNode.attribute("DisabledBgColor"));

    attr_BgState.setValue(childNode.attribute("BgState"));
    attr_Alpha.setValue(childNode.attribute("Alpha"));
    attr_FrameColorBottom.setValue(childNode.attribute("FrameColorBottom"));
    attr_FrameColorTop.setValue(childNode.attribute("FrameColorTop"));
    attr_Clone.setValue(childNode.attribute("Clone"));

    attr_HasFocusDrawStyle.setValue(childNode.attribute("HasFocusDrawStyle"));
    attr_HasNormalDrawStyle.setValue(childNode.attribute("HasNormalDrawStyle"));
    attr_HasDisableDrawStyle.setValue(childNode.attribute("HasDisableDrawStyle"));
    attr_HasNormalPaletteLock.setValue(childNode.attribute("HasNormalPaletteLock"));

    attr_HasFocusPaletteLock.setValue(childNode.attribute("HasFocusPaletteLock"));
    attr_HasDisablePaletteLock.setValue(childNode.attribute("HasDisablePaletteLock"));
    attr_NormalPaletteIndex.setValue(childNode.attribute("NormalPaletteIndex"));
    attr_FocusPaletteIndex.setValue(childNode.attribute("FocusPaletteIndex"));
    attr_DisablePaletteIndex.setValue(childNode.attribute("DisablePaletteIndex"));

    element_StaticWndProperties.setAttributeNode(attr_Text);
    element_StaticWndProperties.setAttributeNode(attr_TextColor);
    element_StaticWndProperties.setAttributeNode(attr_NormalBitmapID);
    element_StaticWndProperties.setAttributeNode(attr_DisabledBitmapID);
    element_StaticWndProperties.setAttributeNode(attr_NormalBgColor);
    element_StaticWndProperties.setAttributeNode(attr_FocusBitmapID);
    element_StaticWndProperties.setAttributeNode(attr_FocusBgColor);
    element_StaticWndProperties.setAttributeNode(attr_DisabledBgColor);

    element_StaticWndProperties.setAttributeNode(attr_BgState);
    element_StaticWndProperties.setAttributeNode(attr_Alpha);
    element_StaticWndProperties.setAttributeNode(attr_FrameColorBottom);
    element_StaticWndProperties.setAttributeNode(attr_FrameColorTop);
    element_StaticWndProperties.setAttributeNode(attr_Clone);
    element_StaticWndProperties.setAttributeNode(attr_HasFocusDrawStyle);
    element_StaticWndProperties.setAttributeNode(attr_HasNormalDrawStyle);
    element_StaticWndProperties.setAttributeNode(attr_HasDisableDrawStyle);
    element_StaticWndProperties.setAttributeNode(attr_HasNormalPaletteLock);

    element_StaticWndProperties.setAttributeNode(attr_HasFocusPaletteLock);
    element_StaticWndProperties.setAttributeNode(attr_HasDisablePaletteLock);
    element_StaticWndProperties.setAttributeNode(attr_NormalPaletteIndex);
    element_StaticWndProperties.setAttributeNode(attr_FocusPaletteIndex);
    element_StaticWndProperties.setAttributeNode(attr_DisablePaletteIndex);
//-----------------------------
    QDomElement element__3DShadowProperties = xmlContents.xmlCreateNode("_3DShadowProperties");

    QDomAttr attr_Enable3DShadow = xmlContents.xmlCreateAttribute(tr("Enable3DShadow"));
    QDomAttr attr_EnableTopLight = xmlContents.xmlCreateAttribute(tr("EnableTopLight"));
    QDomAttr attr_EnableTopLight2 = xmlContents.xmlCreateAttribute(tr("EnableTopLight2"));
    QDomAttr attr_EnableLeftLight = xmlContents.xmlCreateAttribute(tr("EnableLeftLight"));
    QDomAttr attr_EnableLeftLight2 = xmlContents.xmlCreateAttribute(tr("EnableLeftLight2"));
    QDomAttr attr_EnableRightShadow = xmlContents.xmlCreateAttribute(tr("EnableRightShadow"));
    QDomAttr attr_EnableRightShadow2 = xmlContents.xmlCreateAttribute(tr("EnableRightShadow2"));
    QDomAttr attr_EnableBottomShadow = xmlContents.xmlCreateAttribute(tr("EnableBottomShadow"));
    QDomAttr attr_EnableBottomShadow2 = xmlContents.xmlCreateAttribute(tr("EnableBottomShadow2"));
    QDomAttr attr_TopLightColor = xmlContents.xmlCreateAttribute(tr("TopLightColor"));
    QDomAttr attr_TopLight2Color = xmlContents.xmlCreateAttribute(tr("TopLight2Color"));
    QDomAttr attr_LeftLightColor = xmlContents.xmlCreateAttribute(tr("LeftLightColor"));
    QDomAttr attr_LeftLight2Color = xmlContents.xmlCreateAttribute(tr("LeftLight2Color"));
    QDomAttr attr_RightShadowColor = xmlContents.xmlCreateAttribute(tr("RightShadowColor"));
    QDomAttr attr_RightShadow2Color = xmlContents.xmlCreateAttribute(tr("RightShadow2Color"));
    QDomAttr attr_BottomShadowColor = xmlContents.xmlCreateAttribute(tr("BottomShadowColor"));
    QDomAttr attr_BottomShadow2Color= xmlContents.xmlCreateAttribute(tr("BottomShadow2Color"));
    QDomAttr attr_TopLightWidth = xmlContents.xmlCreateAttribute(tr("TopLightWidth"));
    QDomAttr attr_TopLight2Width = xmlContents.xmlCreateAttribute(tr("TopLight2Width"));
    QDomAttr attr_LeftLightWidth = xmlContents.xmlCreateAttribute(tr("LeftLightWidth"));
    QDomAttr attr_LeftLight2Width = xmlContents.xmlCreateAttribute(tr("LeftLight2Width"));
    QDomAttr attr_RightShadowWidth = xmlContents.xmlCreateAttribute(tr("RightShadowWidth"));
    QDomAttr attr_RightShadow2Width = xmlContents.xmlCreateAttribute(tr("RightShadow2Width"));

    QDomAttr attr_BottomShadowWidth= xmlContents.xmlCreateAttribute(tr("BottomShadowWidth"));
    QDomAttr attr_BottomShadow2Width = xmlContents.xmlCreateAttribute(tr("BottomShadow2Width"));
    QDomAttr attr_TopLightAlpha = xmlContents.xmlCreateAttribute(tr("TopLightAlpha"));
    QDomAttr attr_TopLight2Alpha = xmlContents.xmlCreateAttribute(tr("TopLight2Alpha"));
    QDomAttr attr_LeftLightAlpha = xmlContents.xmlCreateAttribute(tr("LeftLightAlpha"));
    QDomAttr attr_LeftLight2Alpha = xmlContents.xmlCreateAttribute(tr("LeftLight2Alpha"));

    QDomAttr attr_RightShadowAlpha = xmlContents.xmlCreateAttribute(tr("RightShadowAlpha"));
    QDomAttr attr_RightShadow2Alpha = xmlContents.xmlCreateAttribute(tr("RightShadow2Alpha"));
    QDomAttr attr_BottomShadowAlpha = xmlContents.xmlCreateAttribute(tr("BottomShadowAlpha"));
    QDomAttr attr_BottomShadow2Alpha = xmlContents.xmlCreateAttribute(tr("BottomShadow2Alpha"));


    childNode = getChildNode(cureNode, "_3DShadowProperties");

    attr_Enable3DShadow.setValue(childNode.attribute("Enable3DShadow"));
    attr_EnableTopLight.setValue(childNode.attribute("EnableTopLight"));
    attr_EnableTopLight2.setValue(childNode.attribute("EnableTopLight2"));
    attr_EnableLeftLight.setValue(childNode.attribute("EnableLeftLight"));

    attr_EnableLeftLight2.setValue(childNode.attribute("EnableLeftLight2"));
    attr_EnableRightShadow.setValue(childNode.attribute("EnableRightShadow"));
    attr_EnableRightShadow2.setValue(childNode.attribute("EnableRightShadow2"));
    attr_EnableBottomShadow.setValue(childNode.attribute("EnableBottomShadow"));

    attr_EnableBottomShadow2.setValue(childNode.attribute("EnableBottomShadow2"));
    attr_TopLightColor.setValue(childNode.attribute("TopLightColor"));
    attr_TopLight2Color.setValue(childNode.attribute("TopLight2Color"));
    attr_LeftLightColor.setValue(childNode.attribute("LeftLightColor"));

    attr_LeftLight2Color.setValue(childNode.attribute("LeftLight2Color"));
    attr_RightShadowColor.setValue(childNode.attribute("RightShadowColor"));
    attr_RightShadow2Color.setValue(childNode.attribute("RightShadow2Color"));
    attr_BottomShadowColor.setValue(childNode.attribute("BottomShadowColor"));

    attr_BottomShadow2Color.setValue(childNode.attribute("BottomShadow2Color"));
    attr_TopLightWidth.setValue(childNode.attribute("TopLightWidth"));
    attr_TopLight2Width.setValue(childNode.attribute("TopLight2Width"));
    attr_LeftLightWidth.setValue(childNode.attribute("LeftLightWidth"));
    attr_LeftLight2Width.setValue(childNode.attribute("LeftLight2Width"));

    attr_RightShadowWidth.setValue(childNode.attribute("RightShadowWidth"));
    attr_RightShadow2Width.setValue(childNode.attribute("RightShadow2Width"));
    attr_BottomShadowWidth.setValue(childNode.attribute("BottomShadowWidth"));
    attr_BottomShadow2Width.setValue(childNode.attribute("BottomShadow2Width"));

    attr_TopLightAlpha.setValue(childNode.attribute("TopLightAlpha"));
    attr_TopLight2Alpha.setValue(childNode.attribute("TopLight2Alpha"));
    attr_LeftLightAlpha.setValue(childNode.attribute("LeftLightAlpha"));
    attr_LeftLight2Alpha.setValue(childNode.attribute("LeftLight2Alpha"));

    attr_RightShadowAlpha.setValue(childNode.attribute("RightShadowAlpha"));
    attr_RightShadow2Alpha.setValue(childNode.attribute("RightShadow2Alpha"));
    attr_BottomShadowAlpha.setValue(childNode.attribute("BottomShadowAlpha"));
    attr_BottomShadow2Alpha.setValue(childNode.attribute("BottomShadow2Alpha"));

    element__3DShadowProperties.setAttributeNode(attr_Enable3DShadow);
    element__3DShadowProperties.setAttributeNode(attr_EnableTopLight);
    element__3DShadowProperties.setAttributeNode(attr_EnableTopLight2);
    element__3DShadowProperties.setAttributeNode(attr_EnableLeftLight);

    element__3DShadowProperties.setAttributeNode(attr_EnableLeftLight2);
    element__3DShadowProperties.setAttributeNode(attr_EnableRightShadow);
    element__3DShadowProperties.setAttributeNode(attr_EnableRightShadow2);
    element__3DShadowProperties.setAttributeNode(attr_EnableBottomShadow);

    element__3DShadowProperties.setAttributeNode(attr_EnableBottomShadow2);
    element__3DShadowProperties.setAttributeNode(attr_TopLightColor);
    element__3DShadowProperties.setAttributeNode(attr_TopLight2Color);
    element__3DShadowProperties.setAttributeNode(attr_LeftLightColor);

    element__3DShadowProperties.setAttributeNode(attr_LeftLight2Color);
    element__3DShadowProperties.setAttributeNode(attr_RightShadowColor);
    element__3DShadowProperties.setAttributeNode(attr_RightShadow2Color);
    element__3DShadowProperties.setAttributeNode(attr_BottomShadowColor);

    element__3DShadowProperties.setAttributeNode(attr_BottomShadow2Color);
    element__3DShadowProperties.setAttributeNode(attr_TopLightWidth);
    element__3DShadowProperties.setAttributeNode(attr_TopLight2Width);
    element__3DShadowProperties.setAttributeNode(attr_LeftLightWidth);
    element__3DShadowProperties.setAttributeNode(attr_LeftLight2Width);

    element__3DShadowProperties.setAttributeNode(attr_RightShadowWidth);
    element__3DShadowProperties.setAttributeNode(attr_RightShadow2Width);
    element__3DShadowProperties.setAttributeNode(attr_BottomShadowWidth);
    element__3DShadowProperties.setAttributeNode(attr_BottomShadow2Width);

    element__3DShadowProperties.setAttributeNode(attr_TopLightAlpha);
    element__3DShadowProperties.setAttributeNode(attr_TopLight2Alpha);
    element__3DShadowProperties.setAttributeNode(attr_LeftLightAlpha);
    element__3DShadowProperties.setAttributeNode(attr_LeftLight2Alpha);

    element__3DShadowProperties.setAttributeNode(attr_RightShadowAlpha);
    element__3DShadowProperties.setAttributeNode(attr_RightShadow2Alpha);
    element__3DShadowProperties.setAttributeNode(attr_BottomShadowAlpha);
    element__3DShadowProperties.setAttributeNode(attr_BottomShadow2Alpha);

//-----------------
    QDomElement element_GradientProperties = xmlContents.xmlCreateNode("GradientProperties");

    QDomAttr attr_NormalFromColor = xmlContents.xmlCreateAttribute(tr("NormalFromColor"));
    QDomAttr attr_NormalToColor = xmlContents.xmlCreateAttribute(tr("NormalToColor"));
    QDomAttr attr_NormalBorderColor = xmlContents.xmlCreateAttribute(tr("NormalBorderColor"));
    QDomAttr attr_FocusFromColor = xmlContents.xmlCreateAttribute(tr("FocusFromColor"));
    QDomAttr attr_FocusToColor = xmlContents.xmlCreateAttribute(tr("FocusToColor"));
    QDomAttr attr_FocusBorderColor = xmlContents.xmlCreateAttribute(tr("FocusBorderColor"));
    QDomAttr attr_DisabledFromColor = xmlContents.xmlCreateAttribute(tr("DisabledFromColor"));
    QDomAttr attr_DisabledToColor = xmlContents.xmlCreateAttribute(tr("DisabledToColor"));
    QDomAttr attr_DisabledBorderColor = xmlContents.xmlCreateAttribute(tr("DisabledBorderColor"));
    QDomAttr attr_GradientType = xmlContents.xmlCreateAttribute(tr("GradientType"));


    childNode = getChildNode(cureNode, "GradientProperties");

    attr_NormalFromColor.setValue(childNode.attribute("NormalFromColor"));
    attr_NormalToColor.setValue(childNode.attribute("NormalToColor"));
    attr_NormalBorderColor.setValue(childNode.attribute("NormalBorderColor"));
    attr_FocusFromColor.setValue(childNode.attribute("FocusFromColor"));
    attr_FocusToColor.setValue(childNode.attribute("FocusToColor"));
    attr_FocusBorderColor.setValue(childNode.attribute("FocusBorderColor"));
    attr_DisabledFromColor.setValue(childNode.attribute("DisabledFromColor"));
    attr_DisabledToColor.setValue(childNode.attribute("DisabledToColor"));
    attr_DisabledBorderColor.setValue(childNode.attribute("DisabledBorderColor"));
    attr_GradientType.setValue(childNode.attribute("GradientType"));

    element_GradientProperties.setAttributeNode(attr_NormalFromColor);
    element_GradientProperties.setAttributeNode(attr_NormalToColor);
    element_GradientProperties.setAttributeNode(attr_NormalBorderColor);
    element_GradientProperties.setAttributeNode(attr_FocusFromColor);
    element_GradientProperties.setAttributeNode(attr_FocusToColor);
    element_GradientProperties.setAttributeNode(attr_FocusBorderColor);
    element_GradientProperties.setAttributeNode(attr_DisabledFromColor);
    element_GradientProperties.setAttributeNode(attr_DisabledToColor);
    element_GradientProperties.setAttributeNode(attr_DisabledBorderColor);
    element_GradientProperties.setAttributeNode(attr_GradientType);

    newNode.appendChild(element_Position);
    newNode.appendChild(element_State);
    newNode.appendChild(element_Text);
    newNode.appendChild(element_Frame);
    newNode.appendChild(element_Border);
    newNode.appendChild(element_Alpha);
    newNode.appendChild(element_Effect);
    newNode.appendChild(element_Navigation);
    newNode.appendChild(element_KeyeventList);
    newNode.appendChild(element_StaticWndProperties);
    newNode.appendChild(element__3DShadowProperties);
    newNode.appendChild(element_GradientProperties);

    if(type == ADD_NEW_BEFORE)
    {
        insertNode.parentNode().insertBefore(newNode, cureNode);
        Menu_Wnd *nwnd = new Menu_Wnd;
        nwnd->node = newNode;
        nwnd->frame = wnd->frame;
        nwnd->parent = wnd->parent;
        nwnd->isMainFrame = false;
        menuWndList.append(nwnd);
        return nwnd;
    }else if(type == ADD_NEW_AFTER)
    {
        insertNode.parentNode().insertAfter(newNode, cureNode);
        Menu_Wnd *nwnd = new Menu_Wnd;
        nwnd->node = newNode;
        nwnd->frame = wnd->frame;
        nwnd->parent = wnd->parent;
        nwnd->isMainFrame = false;
        menuWndList.append(nwnd);
        return nwnd;
    }

    else if(type == ADD_MOVE_UP)
    {
        insertNode.parentNode().insertBefore(newNode, insertNode);
        insertNode.parentNode().removeChild(copywnd->node);
        copywnd->node = wnd->node;
        wnd->node = newNode;
    }else if(type == ADD_MOVE_DOWN)
    {
        insertNode.parentNode().insertAfter(newNode, insertNode);
        insertNode.parentNode().removeChild(copywnd->node);
        copywnd->node = wnd->node;
        wnd->node = newNode;
    }
    return wnd;
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
       foreach(Menu_Wnd * wnd, menuWndList)
       {
           if(wnd->item == item)
           {
               nwnd = add_xmlnode(wnd, wnd, &m_CopyNode, ADD_NEW_BEFORE);
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
}

void xmlExample::on_ptn_moveup_clicked()
{
    Menu_Wnd *curwnd;
    Menu_Wnd *brownd;
    int index;
    QTreeWidgetItem* tempitem;


    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    foreach (QTreeWidgetItem* item, itemlist) {
        tempitem = item;
        curwnd = getMenu_node(item);
        brownd = curwnd->lbroher;
        if(brownd != curwnd)
        {
            index = curwnd->parent->item->indexOfChild(brownd->item);
            curwnd->parent->item->removeChild(item);
            curwnd->parent->item->insertChild(index, tempitem);
            curwnd->item = brownd->item;
            brownd->item = tempitem;

            add_xmlnode(brownd, curwnd, NULL, ADD_MOVE_UP);
        }
    }

}

void xmlExample::on_ptn_movedowm_clicked()
{
    Menu_Wnd *curwnd;
    Menu_Wnd *brownd;
    int index;
    QTreeWidgetItem* tempitem;

    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    foreach (QTreeWidgetItem* item, itemlist) {
        tempitem = item;
        curwnd = getMenu_node(item);
        brownd = curwnd->rbroher;
        if(brownd != curwnd)
        {
            index = curwnd->parent->item->indexOfChild(brownd->item);
            curwnd->parent->item->removeChild(item);
            curwnd->parent->item->insertChild(index, tempitem);
            curwnd->item = brownd->item;
            brownd->item = tempitem;

            add_xmlnode(brownd, curwnd, NULL, ADD_MOVE_DOWN);
        }
    }

}

void xmlExample::removeChild(Menu_Wnd *wnd)
{
    Menu_Wnd *w = wnd->firstChild;
    if(w != wnd)
    {
        while(w->rbroher != w)
        {
            w = w->rbroher;
            removeChild(w->lbroher);
            wnd->node.parentNode().removeChild(w->lbroher->node);
            menuWndList.removeOne(w->lbroher);
            delete w->lbroher;
        }
        removeChild(w);
        wnd->node.parentNode().removeChild(w->node);
        menuWndList.removeOne(w);
        delete w;
    }
}


void xmlExample::on_ptn_delete_clicked()
{
    Menu_Wnd *curwnd;
    QList<QTreeWidgetItem*> itemlist = ui->menu_tree->selectedItems();
    foreach (QTreeWidgetItem* item, itemlist) {
        curwnd = getMenu_node(item);
        removeChild(curwnd);
        curwnd->node.parentNode().removeChild(curwnd->node);
        menuWndList.removeOne(curwnd);

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
}
