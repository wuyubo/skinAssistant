#include "xmlexample.h"
#include "global.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  xmlExample w;
  w.setWindowTitle(QString(VERSION));
  w.setWindowIcon(QIcon("skin.png"));
  w.show();

  return a.exec();
}
/*
能不能生成个过渡文件，
颜色用一些符号代替，比如
ID_COLOR_MAINMENU_BG

后面加个映射 ID_COLOR_MAINMENU_BG : 0xFF00FF

之类的。后面改风格，颜色就简单了。只需要改映射表。。。重新生成XML就好了。。。
*/
