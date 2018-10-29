#include "interface.h"
#include <QFileDialog>

InterFace::InterFace(QObject *parent) : QObject(parent)
{
    pSetting = new Setting(this);
    pSetting->loadSetting();
}

Setting *InterFace::getSetting()
{
    return pSetting;
}
bool InterFace::read_xml()
{
    QFile file(WORK_FILE);

    if (!file.open(QFile::ReadOnly)) {
      qDebug() << "Cannot read file";
      return false;
    }

    m_docElem = xmlContents.xmlRead(&file);
    file.close();
    return true;
}

bool InterFace::load_xml(QString xmlfile)
{
    _xml_file = xmlfile;
    if(_xml_file != NULL)
    {
        copyFileToPath(_xml_file, BACK_UP_FILE, true);
        copyFileToPath(_xml_file, WORK_FILE, true);
        if(read_xml())
        {
            pSetting->setRootPath(getRootPathFromxmlFile(_xml_file));
            parseXml();
            //initTreeWidget();
            //showImge();
            pSetting->saveSetting();
            return true;
        }
        return false;
    }
    return false;
}

bool InterFace::saveXml()
{
    QFile file(WORK_FILE);

    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
      qDebug() << "Cannot open file";
      return false;
    }

    xmlContents.changeSave(&file);
    file.close();
    copyFileToPath(WORK_FILE, _xml_file, true);
    return true;
}

void InterFace::parseXml()
{
    QDomNode n=m_docElem.firstChild();
    QString temp;
    QString header;
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
                   }
               }
           }
           else if(e.tagName () == "ImageList")
           {
                getImageList(e);
           }else if(e.tagName () == "StringMgr")
           {
               getLanguageList(e.firstChild().toElement());
           }
           else if(e.tagName () == "ProjectProperties")
           {
               temp = e.attribute("ImageRootFolder");

               temp.replace("\\", "/");
               header = temp.left(2);
               if(header == "./")
               {
                  temp.replace("./", "");
               }
               pSetting->setImgRelativePath(temp);
               pSetting->setImgPath(pSetting->getRootPath()+temp);
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
   }
}
/////////////////////
void InterFace::GetGWnd(QDomNode node)
{
     QDomNodeList list = node.toElement().childNodes();
     appendFrameList(node.toElement().attribute("Name"));

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

void InterFace::GetWndList(QString frame, QDomNode node)
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
                    wnd->item = NULL;
                    wnd->label = NULL;//new XLabel();
                    wnd->isShow = false;
                    appendMenuWndList(wnd);
                }else
                {
                    wnd2 = new Menu_Wnd();
                    wnd2->node = node2;
                    wnd2->parent = getParentWnd(parentName, wnd);
                    wnd2->isMainFrame = false;
                    wnd2->frame = frame;
                    wnd2->lbroher = getlbrother(wnd2, wnd);
                    wnd2->lbroher->rbroher = wnd2;
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
                    wnd2->item = NULL;
                    wnd2->label = NULL;//new XLabel();
                    wnd2->isShow = false;
                    appendMenuWndList(wnd2);
                    wnd = wnd2;
                }
            }
         }
 }
}

void InterFace::getImageList(QDomElement enode)
{
    QDomNodeList list = enode.childNodes();
    for (int i = 0; i< list.count ();i++)
    {
        QDomNode node2 = list.at(i);
        if (node2.isElement())
        {
           if(node2.toElement().tagName() == "Image")
           {
                XImg *img = getIamge(node2);
                if(img != NULL)
                {
                    img->imgNode = node2;
                    img->ID = node2.toElement().attribute("ID");
                    appendImgList(img);
                }
           }
        }
    }

}
XImg *InterFace::getIamge(QDomNode node)
{
    QDomNodeList list = node.toElement().childNodes();
    XImg *img = new XImg;
    for (int i = 0; i< list.count ();i++)
    {
        QDomNode node2 = list.at(i);
        if (node2.isElement())
        {
            if(node2.toElement().tagName() == "FileName")
            {
                img->FileName = node2.toElement().text();
            }else if(node2.toElement().tagName() == "FolderName")
            {
                img->FolderName = node2.toElement().text();
            }
        }
    }
    return img;

}

void InterFace::getLanguageList(QDomElement enode)
{
    QDomNodeList list = enode.childNodes();
    QString languageId = "0";
    QString languageName = "English";
    for (int i = 0; i< list.count ();i++)
    {
        QDomNode node2 = list.at(i);
        if (node2.isElement())
        {
           if(node2.toElement().tagName() == "Language")
           {
                languageId = node2.toElement().attribute("ID");
                languageName = node2.toElement().attribute("Name");
                QDomNode node3 = node2.firstChild();
                if(node3.isElement())
                {
                    if(node3.toElement().tagName() == "StringList")
                    {
                        getStringList(node3.toElement(), languageId, languageName);
                    }
                }
           }
        }
    }
}

void InterFace::getStringList(QDomElement enode, QString languageId,QString languageName)
{
    QDomNodeList list = enode.childNodes();
    for (int i = 0; i< list.count ();i++)
    {
        QDomNode node2 = list.at(i);
        if (node2.isElement())
        {
           if(node2.toElement().tagName() == "String")
           {
              XString *string = new XString();
              string->strNode = node2;
              string->ID = node2.toElement().attribute("ID");
              string->String = node2.toElement().text();
              string->languageID = languageId;
              string->languageName = languageName;
              appendStringList(string);

           }
        }
    }

}
void InterFace::setCurframe(QString frame)
{
    curframe = frame;
}

QString InterFace::getCurframe()
{
    return curframe;
}

//////////////////////
QString InterFace::getString(QString Id, QString languageName)
{
    foreach (XString *string, StringList) {
        if((string->languageName == languageName)&& (string->ID == Id))
        {
            return string->String;
        }
    }
    return "";
}

Menu_Wnd * InterFace::getMenu_node(QTreeWidgetItem * item)
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

//////////////////////
void InterFace::appendMenuWndList(Menu_Wnd * wnd)
{
    menuWndList.append(wnd);
}
void InterFace::appendImgList(XImg * img)
{
    ImgList.append(img);
}

void InterFace::appendStringList(XString * string)
{
    StringList.append(string);
}
void InterFace::appendFrameList(QString  frame)
{
    frameList.append(frame);
}

//////////////////////////
bool InterFace::copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        qDebug()<<sourceDir;
        qDebug()<<"can not find file !";
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
        qDebug()<<"copy fail";
        qDebug()<<sourceDir;
        qDebug()<<toDir;
        return false;
    }
    return true;
}

QString InterFace::getRootPathFromxmlFile(QString xmlfile)
{
    QStringList strList = xmlfile.split("/");
    if(strList.count() > 0)
    {
        xmlfile.replace(strList[strList.count()-1], "");
    }
    return xmlfile;
}
///////////////////////
Menu_Wnd * InterFace::add_xmlnode(Menu_Wnd *wnd, Menu_Wnd *copywnd, CopyNode *pcnode, ADD_NODE_TYPE type)
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
        if(type == ADD_NEW_CHILD)
        {
            attr_Name.setValue(cureNode.attribute("Name")+pcnode->Name);
        }else
        {
            attr_Name.setValue(wnd->parent->node.toElement().attribute("Name")+pcnode->Name);
        }
    }

    if(type == ADD_NEW_CHILD)
    {
        attr_ParentName.setValue(cureNode.attribute("Name"));
    }
    else
    {
        attr_ParentName.setValue(cureNode.attribute("ParentName"));
    }
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

    if(type == ADD_MOVE_DOWN || type == ADD_MOVE_UP)
    {
        childNode = getChildNode(cureNode, "Navigation");
        tempNode = getChildNode(insertNode, "Navigation");
        attr_Up.setValue(childNode.attribute("Up"));
        attr_Down.setValue(childNode.attribute("Down"));
        attr_Left.setValue(childNode.attribute("Left"));
        attr_Right.setValue(childNode.attribute("Right"));
    }
    else
    {
        attr_Up.setValue(newNode.attribute("Name"));
        attr_Down.setValue(newNode.attribute("Name"));
        attr_Left.setValue(newNode.attribute("Name"));
        attr_Right.setValue(newNode.attribute("Name"));
    }

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
        nwnd->lbroher = wnd->lbroher;
        nwnd->rbroher = wnd;
        nwnd->firstChild = nwnd;
        wnd->lbroher = nwnd;
        nwnd->isMainFrame = false;
        nwnd->isShow = false;
       // nwnd->label = new QLabel(ui->groupBox_ui);
        menuWndList.append(nwnd);
        return nwnd;
    }else if(type == ADD_NEW_AFTER)
    {
        insertNode.parentNode().insertAfter(newNode, cureNode);
        Menu_Wnd *nwnd = new Menu_Wnd;
        nwnd->node = newNode;
        nwnd->frame = wnd->frame;
        nwnd->parent = wnd->parent;
        nwnd->lbroher = wnd;
        nwnd->rbroher = wnd->rbroher;
        nwnd->firstChild = nwnd;
        wnd->rbroher = nwnd;
        nwnd->isMainFrame = false;
        nwnd->isShow = false;
        //nwnd->label = new QLabel(ui->groupBox_ui);
        menuWndList.append(nwnd);
        return nwnd;
    }else if(type == ADD_NEW_CHILD)
    {
        Menu_Wnd *lastchild = getWndLastChild(wnd);
        insertNode.parentNode().insertAfter(newNode, lastchild->node.toElement());
        Menu_Wnd *nwnd = new Menu_Wnd;
        nwnd->node = newNode;
        nwnd->frame = wnd->frame;
        nwnd->parent = wnd;
        nwnd->lbroher = lastchild;
        nwnd->rbroher = nwnd;
        nwnd->firstChild = nwnd;
        nwnd->isMainFrame = false;
        nwnd->isShow = false;
       // nwnd->label = new QLabel(ui->groupBox_ui);
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

Menu_Wnd *InterFace::getParentWnd(QString parenName, Menu_Wnd *prevWnd)
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
Menu_Wnd * InterFace::getlbrother(Menu_Wnd *curwnd, Menu_Wnd *prewnd)
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

Menu_Wnd* InterFace::getWndLastChild(Menu_Wnd *wnd)
{
    Menu_Wnd *child = wnd->firstChild;
    if(child == wnd)
    {
        return wnd;
    }
    while(child->rbroher != child)
    {
        child = child->rbroher;
    }
    return child;
}

QDomElement InterFace::getChildNode(QDomElement node, QString childName)
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
void InterFace::removeChild(Menu_Wnd *wnd)
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

Menu_Wnd *InterFace::getWndFromId(QString ID)
{
    foreach(Menu_Wnd * wnd, menuWndList)
    {
       if(wnd->node.toElement().attribute("Name") == ID)
        {
            return wnd;
        }
    }
    return NULL;
}
///////////////////
/// \brief InterFace::cloneNode
/// \param node
/// \param cnode
/*
void InterFace::cloneNode(QDomNode node, QDomNode cnode)
{
    QDomNode n = node.firstChild();
    QDomNode n2 = cnode.firstChild();

    while (!n.isNull () && !n2.isNull ())
    {
       if (n.isElement())
       {
           QDomElement e = n.toElement ();
           QDomElement e2 = getChildNode(n2.toElement(), e.tagName ());
           if(e.tagName () == "Position" && ui->cb_display)
           {
                e.toElement().setAttribute("X", e2.attribute("X"));
                //e.toElement().setAttribute("Y", e2.attribute("Y"));
                e.toElement().setAttribute("Width",e2.attribute("Width"));
                e.toElement().setAttribute("Height", e2.attribute("Height"));
           }
           if(e.tagName () == "Text" && ui->cb_text)
           {
                e.toElement().setAttribute("TextAlign",e2.attribute("TextAlign"));
                e.toElement().setAttribute("NormalTextColor", e2.attribute("NormalTextColor"));
                e.toElement().setAttribute("FocusTextColor", e2.attribute("FocusTextColor"));
                e.toElement().setAttribute("DisableTextColor",e2.attribute("DisableTextColor"));
           }
           if(e.tagName () == "StaticWndProperties" && ui->cb_advanced)
           {

                e.toElement().setAttribute("Clone", e2.attribute("Clone"));

                e.toElement().setAttribute("NormalBgColor", e2.attribute("NormalBgColor"));
                e.toElement().setAttribute("FocusBgColor", e2.attribute("FocusBgColor"));
                e.toElement().setAttribute("DisabledBgColor", e2.attribute("DisabledBgColor"));
                e.toElement().setAttribute("NormalBitmapID", e2.attribute("NormalBitmapID"));
                e.toElement().setAttribute("FocusBitmapID", e2.attribute("FocusBitmapID"));
                e.toElement().setAttribute("DisabledBitmapID", e2.attribute("DisabledBitmapID"));
                e.toElement().setAttribute("BgState", e2.attribute("BgState"));
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点
    }
}
*/
IMG *InterFace::add_xmlImg(IMG *lastImg, IMG *tagImg)
{
    QDomElement cureNode = lastImg->imgNode.toElement();
    QDomElement newNode = xmlContents.xmlCreateNode("Image");

    QDomAttr attr_ColorKeyEnd = xmlContents.xmlCreateAttribute(tr("ColorKeyEnd"));
    QDomAttr attr_FileType = xmlContents.xmlCreateAttribute(tr("FileType"));
    QDomAttr attr_ColorKeyFrom = xmlContents.xmlCreateAttribute(tr("ColorKeyFrom"));
    QDomAttr attr_AlphaFrom = xmlContents.xmlCreateAttribute(tr("AlphaFrom"));
    QDomAttr attr_CvtFormat = xmlContents.xmlCreateAttribute(tr("CvtFormat"));
    QDomAttr attr_EnableClrKey = xmlContents.xmlCreateAttribute(tr("EnableClrKey"));
    QDomAttr attr_ID = xmlContents.xmlCreateAttribute(tr("ID"));
    QDomAttr attr_MENU_IMG_INFO_FOCUS = xmlContents.xmlCreateAttribute(tr("MENU_IMG_INFO_FOCUS"));
    attr_ColorKeyEnd.setValue(cureNode.attribute("ColorKeyEnd"));
    attr_FileType.setValue(cureNode.attribute("FileType"));
    attr_ColorKeyFrom.setValue(cureNode.attribute("ColorKeyFrom"));

    attr_AlphaFrom.setValue(cureNode.attribute("AlphaFrom"));
    attr_CvtFormat.setValue(cureNode.attribute("CvtFormat"));
    attr_EnableClrKey.setValue(cureNode.attribute("EnableClrKey"));

    attr_ID.setValue(tagImg->ID);
    attr_MENU_IMG_INFO_FOCUS.setValue(cureNode.attribute("MENU_IMG_INFO_FOCUS"));

    newNode.setAttributeNode(attr_ColorKeyEnd);
    newNode.setAttributeNode(attr_FileType);
    newNode.setAttributeNode(attr_ColorKeyFrom);
    newNode.setAttributeNode(attr_AlphaFrom);

    newNode.setAttributeNode(attr_CvtFormat);
    newNode.setAttributeNode(attr_EnableClrKey);
    newNode.setAttributeNode(attr_ID);
    newNode.setAttributeNode(attr_MENU_IMG_INFO_FOCUS);
//----------------------
    QDomElement element_FileName = xmlContents.xmlCreateNode("FileName");

    element_FileName.setNodeValue(tagImg->FileName);

    QDomElement element_FolderName = xmlContents.xmlCreateNode("FolderName");
    QString folder;
    if(tagImg->FolderName == pSetting->getImgPath())
    {
        folder = "..\\..\\bitmap1366x768x565";
    }else
    {
        folder = tagImg->FolderName.replace(pSetting->getImgPath(), "");
        folder = folder.replace("/", "\\");
    }
     element_FolderName.setNodeValue(folder);

     newNode.appendChild(element_FileName);
     newNode.appendChild(element_FolderName);
     cureNode.parentNode().insertAfter(newNode, cureNode);

     tagImg->imgNode = newNode;
     return tagImg;
}

bool InterFace::checkImg(QString imgName)
{
    QStringList namelist = imgName.split(".");
    QString ID, suffix;
    if(namelist.length() < 2)
    {
        //showtips("请添加添加正确的img/png");
        return false;
    }
    ID = namelist[0];
    suffix = namelist[1];
    if((suffix != "bmp" && suffix != "png")&&(suffix != "BMP" && suffix != "PNG"))
    {
        //showtips(ID+suffix+"添加的不是bmp/png");
        return false;
    }
    foreach (IMG *img, ImgList)
    {
        if(img->ID == ID)
        {
            //showtips(ID + "已经存在");
            return false;
        }
    }
    return true;
}

bool InterFace::addImge(QString imgPath)
{
    IMG *img = NULL;
    QString rootPath = "";
    QString imgName = "";
    QStringList strlist = imgPath.split("/");
    QStringList namelist;
    for(int i = 0; i < strlist.length(); i++)
    {
        if(i == strlist.length()-1)
        {
            imgName = strlist[i];
        }
        else
        {
            rootPath += strlist[i]+"/";
        }
    }

   if(checkImg(imgName))
   {
       namelist = imgName.split(".");
       img = new IMG;
       img->FileName = imgName;
       img->FolderName = rootPath;
       img->ID = namelist[0];
       ImgList.append(add_xmlImg(ImgList.last(), img));
       return true;
   }
    return false;
}
/*
void InterFace::cloneTree(Menu_Wnd * wnd, Menu_Wnd * ctree)
{
    Menu_Wnd *child = wnd->firstChild;
    Menu_Wnd *child2 = ctree->firstChild;
    cloneNode(wnd->node, ctree->node);

    while(child->rbroher != child && child2->rbroher != child2)
    {
        if(child->firstChild != child && child2->firstChild != child2)
        {
            cloneTree(child, child2);
        }
        else
        {
            cloneNode(child->node, child2->node);
        }
        child = child->rbroher;
        child2 = child2->rbroher;
    }
}*/

void  InterFace::cloneWndAttr(Menu_Wnd *wnd, CNode cnode, CNode _CopyNode)
{
    QDomNode n = wnd->node.firstChild();
    QDomNode pn = wnd->parent->node;
    bool ok;

    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Position" && (_CopyNode.type == CALL || _CopyNode.isDisplay))
           {
               int x = e.attribute("x").toInt(&ok);
               int y = e.attribute("y").toInt(&ok);
               int w = e.attribute("Width").toInt(&ok);
               int h = e.attribute("Height").toInt(&ok);

               int cx = cnode.position.X;
               int cy = cnode.position.Y;
               int cw = cnode.position.Width;
               int ch = cnode.position.Height;
               int cpx = cnode.position.Px;
               int cpy = cnode.position.Py;

               int temp;
               QDomElement pe = getChildNode(pn.toElement(), e.tagName());
               int px = pe.attribute("X").toInt(&ok);
               int py = pe.attribute("Y").toInt(&ok);

               if(CALIGN != _CopyNode.type)
               {

                   if(cw > -1)
                   {
                        e.toElement().setAttribute("Width",cw);
                   }
                   if(ch > -1)
                   {
                        e.toElement().setAttribute("Height", ch);
                   }
               }

               switch (_CopyNode.Align) {
               case ALIGN_LEFT:
                   e.toElement().setAttribute("X", cx);
                   break;
               case ALIGN_RIGHT:
                   temp = cx+(cw-w);
                   e.toElement().setAttribute("X", temp);
                   break;
               case ALIGN_CENTER:
                   temp = cx+(cw/2-w/2);
                   e.toElement().setAttribute("X", temp);
                   break;
               case ALIGN_PARENT:
                   temp = px + (cx-cpx);
                   e.toElement().setAttribute("X", temp);
                   temp = py + (cy-cpy);
                   e.toElement().setAttribute("Y", temp);
                   break;
               case ALIGN_UP:
                   e.toElement().setAttribute("Y", cy);
                   break;
               case ALIGN_DOWN:
                   temp = cy+(ch-h);
                   e.toElement().setAttribute("Y", temp);
                   break;
               case ALIGN_CENTER_UD:
                   temp = cy+(ch/2-h/2);
                   e.toElement().setAttribute("Y", temp);
                   break;
               case ALIGN_POSITION:
                   if(cx > -1)
                   {
                        e.toElement().setAttribute("X", cx);
                   }
                   if(cy > -1)
                   {
                        e.toElement().setAttribute("Y", cy);
                   }
                   break;
               case ALIGN_RESIZE:
                   if(cw > -1)
                   {
                        e.toElement().setAttribute("Width",cw);
                   }
                   if(ch > -1)
                   {
                        e.toElement().setAttribute("Height", ch);
                   }
                   break;
               default:
                   if(cx > -1)
                   {
                        e.toElement().setAttribute("X", cx);
                   }
                   if(cy > -1)
                   {
                        e.toElement().setAttribute("Y", cy);
                   }
                   break;
               }
           }
           if(e.tagName () == "Text"&& (_CopyNode.type == CALL || _CopyNode.isText))
           {
                e.toElement().setAttribute("TextAlign",cnode.text.TextAlign);
                e.toElement().setAttribute("NormalTextColor", cnode.text.NormalTextColor);
                e.toElement().setAttribute("FocusTextColor", cnode.text.FocusTextColor);
                e.toElement().setAttribute("DisableTextColor",cnode.text.DisableTextColor);
           }
           if(e.tagName () == "StaticWndProperties" && (_CopyNode.type == CALL || _CopyNode.isAdvanced))
           {
                e.toElement().setAttribute("Clone", cnode.properties.Clone);

                e.toElement().setAttribute("NormalBgColor", cnode.properties.NormalBgColor);
                e.toElement().setAttribute("FocusBgColor", cnode.properties.FocusBgColor);
                e.toElement().setAttribute("DisabledBgColor", cnode.properties.DisabledBgColor);
                e.toElement().setAttribute("NormalBitmapID", cnode.properties.NormalBitmapID);
                e.toElement().setAttribute("FocusBitmapID", cnode.properties.FocusBitmapID);
                e.toElement().setAttribute("DisabledBitmapID", cnode.properties.DisabledBitmapID);
                e.toElement().setAttribute("BgState", cnode.properties.BgState);

                e.toElement().setAttribute("HasNormalDrawStyle", cnode.properties.HasNormalDrawStyle);
                e.toElement().setAttribute("HasFocusDrawStyle", cnode.properties.HasFocusDrawStyle);
                e.toElement().setAttribute("HasDisableDrawStyle", cnode.properties.HasDisableDrawStyle);
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}

QString InterFace::getWinIamgeId(Menu_Wnd *wnd, ITEM_STATUS status)
{
    QDomNode n = wnd->node.firstChild();
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();
           if(e.tagName () == "StaticWndProperties")
           {
               if(status == ST_NOMAL)
               {
                   return e.toElement().attribute("NormalBitmapID");
               }else if(status == ST_FOCUS)
               {
                   return e.toElement().attribute("FocusBitmapID");
               }else if(status == ST_DISABLE)
               {
                   return e.toElement().attribute("DisabledBitmapID");
               }
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
    return "";
}

void InterFace::setWinIamgeId(Menu_Wnd *wnd, QString imgID, ITEM_STATUS status)
{
    QDomNode n = wnd->node.firstChild();
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();
           if(e.tagName () == "StaticWndProperties")
           {
               if(status == ST_NOMAL)
               {
                   e.toElement().setAttribute("NormalBitmapID", imgID);
               }else if(status == ST_FOCUS)
               {
                   e.toElement().setAttribute("FocusBitmapID", imgID);
               }else if(status == ST_DISABLE)
               {
                   e.toElement().setAttribute("DisabledBitmapID", imgID);
               }
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}



bool InterFace::isCheckImgFromID(QString imgID)
{
    foreach (XImg *img, ImgList) {
        if(img->ID == imgID)
        {
            return true;
        }
    }
    return false;
}

bool InterFace::removeImg(XImg *img)
{
    img->imgNode.parentNode().removeChild(img->imgNode);
    ImgList.removeOne(img);

    return true;
}

QString InterFace::getWinStrId(Menu_Wnd *wnd)
{
    QDomNode n = wnd->node.firstChild();
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();

           if(e.tagName () == "Text")
           {
                return e.toElement().attribute("TextID");
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
    return "";
}
void InterFace::setWinStrId(Menu_Wnd *wnd, QString imgID)
{
    QDomNode n = wnd->node.firstChild();
    while (!n.isNull ())
    {
       if (n.isElement ())
       {
           QDomElement e = n.toElement ();
           if(e.tagName () == "Text")
           {
                e.toElement().setAttribute("TextID",imgID);
           }
       }
       n = n.nextSibling ();//nextSibling()获取下一个兄弟节点

    }
}

bool InterFace::removeStr(XString *str)
{
    str->strNode.parentNode().removeChild(str->strNode);
    StringList.removeOne(str);

    return true;
}
