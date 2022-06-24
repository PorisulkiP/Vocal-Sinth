#ifndef PARTSTIMELINEGRAPHVIEW_H
#define PARTSTIMELINEGRAPHVIEW_H

#include <future>

#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QTimer>
#include <QThread>
#include <QDebug>

#include "project.h"
#include "variables.h"
#include "Indicator.h"

class PartsTimelineGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    PartsTimelineGraphicsView(QWidget *parent = nullptr){
        setScene(new QGraphicsScene());
        creating();
    }
    PartsTimelineGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~PartsTimelineGraphicsView();

    void setSceneX(qreal x) { setSceneRect(x, sceneRect().y(), sceneRect().width(), sceneRect().height()); }
    void setSceneY(qreal y) { setSceneRect(sceneRect().x(), y, sceneRect().width(), sceneRect().height()); }
    void setSceneW(qreal w) { setSceneRect(sceneRect().x(), sceneRect().y(), w, sceneRect().height());     }
    void setSceneH(qreal h) { setSceneRect(sceneRect().x(), sceneRect().y(), sceneRect().width(), h);      }

    Indicator *indicator = new Indicator();

    void creating();
    void createTimer();

    void startAnimate();
    void stopAnimate();

    void setPressed(bool newMode){ _pressed = newMode; }

    bool pressed(){ return _pressed; }

signals:
    void newTime(int indPos);
    void stopPlaying();

protected:
    QPoint newPos, oldPos, oldMousePos;
    QPoint _indicatorPos;
    QGraphicsItem *item;
    QTimer* pTimer = new QTimer();

    // переменная для таймера
    float newXPos = 0;

    int xOffset = gPRXOffset; // Растояние между цифрами на таймлайне
    int remaining = 0;
    int _xDiff = 0;
    qreal ins = 0;

    bool _pressed = false;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
};
#endif // PARTSTIMELINEGRAPHVIEW_H
