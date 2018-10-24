#include "xlabel.h"

XLabel::XLabel(QWidget *parent)
    : QLabel(parent)
{
    n= -1;
    m_bStartDrawFlag = false;
}

void XLabel::paintEvent(QPaintEvent *event)
{
    if (m_bStartDrawFlag)
    {
        QLabel::paintEvent(event);//先调用父类的paintEvent为了显示'背景'!!!
        QPainter painter(this);
        for (auto iter = m_listCenter.begin(); iter != m_listCenter.end(); iter++)
        {
            painter.setPen(QPen(Qt::red,2));

            painter.drawRect(iter->x()-4, iter->y()-4, 8, 8);
        }
    }
}

void XLabel::mousePressEvent ( QMouseEvent * event )
{
    if (Qt::LeftButton == event->button())
    {
        n = -1;
        for(int i = 0; i<m_listCenter.length(); i++)
        {
            int x = event->x();
            int y =event->y();
            int  nx= abs(m_listCenter[i].x()-event->x());
            int ny =abs(m_listCenter[i].y() - event->y());
            if (nx <=4 && ny<=4)
            {
                n=i;
            }
        }
    }
}

void XLabel::mouseReleaseEvent ( QMouseEvent * event )
{
    if (n != -1)
    {
        m_listCenter.replace(n, event->pos());
        update();
    }
}

void XLabel::AddPoint(QPoint point)
{
    m_listCenter.push_back(point);
}

void XLabel::ClearAllPoint()
{
    m_listCenter.clear();
}

XLabel::~XLabel()
{

}
