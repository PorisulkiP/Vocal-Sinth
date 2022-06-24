#ifndef PR_TIMELINEGRAPHVIEW_H
#define PR_TIMELINEGRAPHVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>

#include "variables.h"
#include "Indicator.h"

class PRTimelineGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:

    PRTimelineGraphicsView(QWidget *parent = nullptr){
        setScene(new QGraphicsScene());
        creating();
    }
    ~PRTimelineGraphicsView();

    void setSceneX(qreal x) { setSceneRect(x, sceneRect().y(), sceneRect().width(), sceneRect().height()); }
    void setSceneY(qreal y) { setSceneRect(sceneRect().x(), y, sceneRect().width(), sceneRect().height()); }
    void setSceneW(qreal w) { setSceneRect(sceneRect().x(), sceneRect().y(), w, sceneRect().height());     }
    void setSceneH(qreal h) { setSceneRect(sceneRect().x(), sceneRect().y(), sceneRect().width(), h);      }

    QPoint newPos, oldPos, oldMousePos;
    QPoint startTimelinePoint = QPoint(0,0);
    QGraphicsItem *item;

    // Переменные для каретки времени
    Indicator *indicator = new Indicator();
    QTimer* pTimer = new QTimer();

    int xOffset = gPRXOffset; // Растояние между цифрами на таймлайне
    int remaining = 0;
    int _xDiff = 0;
    qreal ins = 0;

    // переменные для таймера
    float updatePosInterval = 0;
    float newXPos = 0;

    void creating();
    void scrollCall(int dx, int dy);
    void createTimer();

    void startAnimate();
    void stopAnimate();

    void setPressed(bool newMode){ _pressed = newMode; }

    bool pressed(){ return _pressed; }

signals:
    void newTime(int indPos);
    void stopPlaying();

protected:
    bool _pressed = false;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void scrollContentsBy(int dx, int dy) override;
};
#endif // PR_TIMELINEGRAPHVIEW_H
