#include "xmlexample.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  xmlExample w;
  w.show();

  return a.exec();
}
