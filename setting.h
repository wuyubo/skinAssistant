#ifndef SETTING_H
#define SETTING_H

#include <QObject>

class Setting : public QObject
{
    Q_OBJECT
public:
    explicit Setting(QObject *parent = 0);
    QString readFile(QString path);
    bool saveFile(QString path, QString data, bool isCover = true);
    void defaultSetting();
    bool loadSetting();
    bool saveSetting();

    void setRootPath(QString path);
    QString getRootPath();
    void setImgPath(QString path);
    QString getImgPath();
    void setStringPath(QString path);
    QString getStringPath();
    void setImgRelativePath(QString path);
    QString getImgRelativePath();

signals:

public slots:
private:
    QString _rootPath;
    QString _imgPath;
    QString _imgRelativePath;
    QString _stringPath;

};

#endif // SETTING_H
