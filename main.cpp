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
