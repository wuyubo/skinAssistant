#include "zxml.h"

ZXml::ZXml()
{
}

QDomElement ZXml::xmlRead(QIODevice *device)
{
    m_doc.clear();
  //xml.setDevice(device); // Set read position to start of file
    if (!m_doc.setContent(device)) {

    }

  return m_doc.documentElement();
}

QString ZXml::errorString() const
{
  return QObject::tr("%1\nLine %2, column %3")
      .arg(xml.errorString())
      .arg(xml.lineNumber())
      .arg(xml.columnNumber());
}

void ZXml::readVersion()
{
    QString version;
    Q_ASSERT(xml.isStartElement() && xml.name() == "Version");
    version = xml.readElementText();
    qDebug() << version;
    m_project.Version = version;
}

void ZXml::readBuild()
{
    QString Build;
    Q_ASSERT(xml.isStartElement() && xml.name() == "Build");
    Build = xml.readElementText();
    qDebug() << Build;
    m_project.Build = Build;
}
void ZXml::readWnd(GWnd *pgwnd)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "Wnd");
    //bool ok;
    Wnd  wnd;
    QString strtemp;
    strtemp = xml.attributes().value("Name").toString();
    wnd.Name = strtemp;
    qDebug() << strtemp;
    strtemp = xml.attributes().value("ParentName").toString();
    wnd.ParentName = strtemp;

    pgwnd->wndlist.wndList.append(wnd);
}
void ZXml::readWndList(GWnd *pgwnd)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "WndList");
    while (xml.readNextStartElement()) {
      if (xml.name() == "Wnd")
      {
        qDebug() << xml.name();
        readWnd(pgwnd);
      }
      xml.skipCurrentElement();
    }
}

void ZXml::readGWnd()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "GWnd");
    bool ok;
    GWnd  gwnd;
    QString strtemp;
    int iTemp;
    strtemp = xml.attributes().value("Name").toString();
    gwnd.Name = strtemp;
    iTemp = xml.attributes().value("XPos").toInt(&ok);
    gwnd.XPos = iTemp;
    iTemp = xml.attributes().value("YPos").toInt(&ok);
    gwnd.YPos = iTemp;
    iTemp = xml.attributes().value("Width").toInt(&ok);
    gwnd.Width = iTemp;
    iTemp = xml.attributes().value("Height").toInt(&ok);
    gwnd.Height = iTemp;
    strtemp = xml.attributes().value("GWIN").toString();
    gwnd.GWIN = strtemp;
    while (xml.readNextStartElement()) {
      if (xml.name() == "WndList")
      {
        qDebug() << xml.name();
        readWndList(&gwnd);
      }
      else
         xml.skipCurrentElement();
    }

    m_project.gwndList.gwndlist.append(gwnd);
}

void ZXml::readGWndList()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "GWndList");
    while (xml.readNextStartElement()) {
      if (xml.name() == "GWnd")
      {
        qDebug() << xml.name();
        readGWnd();
      }
      else
        xml.skipCurrentElement();
    }
}

void ZXml::readXmlContents()  // Pick off second level xml tags
{
  qDebug() << "Contents:";
   m_numberInfo.numberList.clear();
  Q_ASSERT(xml.isStartElement() && xml.name() == XMLDOCTYPE);

  while (xml.readNextStartElement()) {
    if (xml.name() == "Version") {
      qDebug() << xml.name();
      readVersion();
    }else if(xml.name() == "Build")
    {
       readBuild();
    }
    else if(xml.name() == "GWndList")
    {
       readGWndList();
    }
    else {
      xml.skipCurrentElement();
    }
  }
}

void ZXml::readNumber()
{
  Q_ASSERT(xml.isStartElement() && xml.name() == "Number");
  bool ok;
  XmlNumber numberNode;
  int value = xml.attributes().value("value").toInt(&ok);
  if (ok) {
    qDebug() << value;
    numberNode.value = QString::number(value);
  } else {
    qDebug() << "Value not found.";
  }

  while (xml.readNextStartElement()) {
    if (xml.name() == "Name")
      numberNode.name = readName();
    else if (xml.name() == "Roman")
      numberNode.Roman = readRoman();
    else
      xml.skipCurrentElement();
  }
  m_numberInfo.numberList.append(numberNode);
}

QString ZXml::readName()
{
  QString name;
  Q_ASSERT(xml.isStartElement() && xml.name() == "Name");
  name = xml.readElementText();
  qDebug() << name;
  return name;
}

QString ZXml::readRoman()
{
  QString roman;
  Q_ASSERT(xml.isStartElement() && xml.name() == "Roman");
  roman = xml.readElementText();
  qDebug() << roman;
  return roman;
}

QString ZXml::readValue()
{
  QString value;
  Q_ASSERT(xml.isStartElement() && xml.name() == "Value");
  value = xml.readElementText();
  qDebug() <<  value;
  return value;
}

XSkinToolProject * ZXml::getSkinToolProject()
{
    return &m_project;
}

XmlNumberInfo* ZXml::getNumberInfo()
{
    return &m_numberInfo;
}

void ZXml::xmlWrite(QIODevice *device)
{
    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("NumberInfo");
    foreach(XmlNumber number, m_numberInfo.numberList)
    {
        writer.writeStartElement("number");
        writer.writeTextElement("Name",number.name);
        writer.writeTextElement("Roman",number.Roman);
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndDocument();
}

bool ZXml::changeSave(QIODevice *device)
{
    QTextStream ts(device);
    ts.reset();
    ts.setCodec("utf-8");
    m_doc.save(ts, 4);
    return true;
}

QDomElement ZXml::xmlCreateNode(QString tagName)
{
    return m_doc.createElement(tagName);
}

