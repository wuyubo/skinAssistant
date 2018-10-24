#include "setting.h"
#include "global.h"
#include <QFile>
Setting::Setting(QObject *parent) : QObject(parent)
{
    defaultSetting();
}


QString Setting::readFile(QString path)
{
    QString data="";
    char cBuf[128];
    qint64 LineLen;
    QFile file(path);      //---打开文件
    data.clear();
    if (file.open(QIODevice :: ReadOnly)) //  以只读的方式打开
    {
        do
        {
            LineLen = file.readLine(cBuf, sizeof(cBuf)); //---读取文本文件的一行
            if (-1 != LineLen)                          //---读取成功，将返回读取的字节，读取失败，将返回-1
            {
                data.append(QString(QLatin1String(cBuf)));
            }
        }while(-1 != LineLen);
    }
    file.close();
    return data;
}

bool Setting::saveFile(QString path, QString data, bool isCover)
{
    QFile file(path);
    isCover = isCover;//not uses
    if (file.open(QIODevice::WriteOnly)) //打开方式：可写、二进制方式
    {
        //按二进制写入时，须进行类型转换，这也是QFile使用上比较麻烦的地方
        file.write(data.toStdString().c_str()); //参数为char*，须转换
        file.close();
        return true;
    }
    return false;
}

void Setting::defaultSetting()
{
    _rootPath = XML_PATH;
    _imgPath  = "";
    _stringPath = "";
}

bool Setting::loadSetting()
{
    QString data = readFile(SETTING_FILE);
    if(data == "")
    {
        return false;
    }
    QStringList dataList, setlist;
    data.replace(QString(" "), QString(""));
    dataList = data.split("\n");
    foreach (QString v, dataList)
    {
        setlist = v.split("=");
        if(setlist.length() > 1)
        {
            if(setlist[0] == "rootpath")
            {
                setRootPath(setlist[1]);
            }else if(setlist[0] == "imgPath")
            {
                setImgPath(setlist[1]);
            }else if(setlist[0] == "stringPath")
            {
                setImgPath(setlist[1]);
            }
        }
    }
    return true;
}

bool Setting::saveSetting()
{
    QString data = "";
    data.append("rootpath=").append(getRootPath()).append("\r\n");
    data.append("imgPath=").append(getImgPath()).append("\r\n");
    data.append("stringPath=").append(getStringPath()).append("\r\n");
    saveFile(SETTING_FILE, data, true);
}

void Setting::setRootPath(QString path)
{
    _rootPath = path;
}

QString Setting::getRootPath()
{
    return _rootPath;
}

void Setting::setImgPath(QString path)
{
    _imgPath = path;
}

QString Setting::getImgPath()
{
    return _imgPath;
}

void Setting::setStringPath(QString path)
{
    _stringPath = path;
}

QString Setting::getStringPath()
{
    return _stringPath;
}
void Setting::setImgRelativePath(QString path)
{
    _imgRelativePath = path;
}

QString Setting::getImgRelativePath()
{
    return _imgRelativePath;
}

