#include "otherfunction.h"
#include "ui_otherfunction.h"

OtherFunction::OtherFunction(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OtherFunction)
{
    ui->setupUi(this);
    output.clear();
    defineList.clear();
}

OtherFunction::~OtherFunction()
{
    delete ui;
}
#define RcGetOsdNum_X(xpos_1080p)     ((xpos_1080p)/2)			//  1/2 == 960/1920
#define RcGetOsdNum_Y(ypos_1080p)     ((ypos_1080p)/2)			//  1/2 == 540 /1080
#define ALIGN_8(_x_)                (((_x_) + 7) & ~7)
void OtherFunction::MApp_E_OSD_RestTransition(int x, int y, int w, int h)
{
    QString temp;
    int number;
    int oldnumber;
    int oldx =x, oldy=y, oldw=w, oldh=h;
    w = RcGetOsdNum_X(x+w) - RcGetOsdNum_X(x);
    h = RcGetOsdNum_Y(y+h) - RcGetOsdNum_Y(y);
    x = RcGetOsdNum_X(x);
    y = RcGetOsdNum_Y(y);

    w = ALIGN_8(w);
    h = ALIGN_8(h);
    for(int i = 0; i < defineList.count(); i++)
    {
        if(i == 0)
        {
           number = x;
           oldnumber = oldx;
        }else if(i == 1)
        {
           number = y;
           oldnumber = oldy;
        }else if(i == 2)
        {
           number = w;
           oldnumber = oldw;
        }else if(i == 3)
        {
           number = h;
           oldnumber = oldh;
        }

        temp.append("#undef ").append(defineList[i]).append("\n");
        temp.append("#define ").append(defineList[i]).append(" ").append(QString::number(number)).
                append("//").append(QString::number(oldnumber)).append("\n");
    }
    temp.append("\n");
    output.append(temp);

}

void OtherFunction::on_ptn_define_clicked()
{
    QString input = ui->textEdit_input->toPlainText();
    QStringList inputList = input.split("\n", QString::SkipEmptyParts);
    QStringList templist;
    QString temp;
    bool ok;
    output.clear();
    int x, y, w, h;
    for(int i = 0; i<inputList.count(); i++)
    {
        templist = inputList[i].split(":", QString::SkipEmptyParts);
        if(templist.count() > 2)
        {
            temp = templist[2];
        }else
        {
            temp = templist[1];
        }
        templist = temp.split(" ", QString::SkipEmptyParts);
        if(templist.count()>2)
        {
            temp = templist[templist.count()-1];
            int index = (i+1)%4;
            int number = temp.toInt(&ok);
            defineList.append(templist[1]);
            if(index == 1)
            {
                x = number;
            }else if(index == 2)
            {
                y = number;
            }else if(index == 3)
            {
                w = number;
            }else if(index == 0)
            {
                h = number;
                MApp_E_OSD_RestTransition(x,y,w,h);
                defineList.clear();
            }
        }
    }
    ui->textEdit_output->setText(output);
}
