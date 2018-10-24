#ifndef XLABEL_H
#define XLABEL_H

#include <QObject>
#include <QLabel>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QList>
class XLabel : public QLabel
{
    Q_OBJECT
    public:
        //鼠标拖动当前特征点
        int n;

        QList<QPoint> m_listCenter;//存储中心点坐标

        bool m_bStartDrawFlag;//开始画图标志位
    public:
        XLabel(QWidget *parent = 0);

        ~XLabel();

        void AddPoint(QPoint point);

        void ClearAllPoint();
    protected:
        void paintEvent(QPaintEvent *event);

        void mousePressEvent ( QMouseEvent * event );

        void mouseReleaseEvent ( QMouseEvent * event );
};

#endif // XLABEL_H
