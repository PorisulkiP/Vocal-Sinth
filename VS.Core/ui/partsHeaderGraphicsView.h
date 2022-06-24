#ifndef PARTSHEADERGRAPHVIEW_H
#define PARTSHEADERGRAPHVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>

class PartsHeaderGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    PartsHeaderGraphicsView(QWidget *parent = nullptr)
    {
        setScene(new QGraphicsScene());
        setMinimumWidth(200);
    }
    PartsHeaderGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~PartsHeaderGraphicsView();

    QPoint newPos, oldPos, oldMousePos;
    QGraphicsView *body_view = 0;

    bool body_call = 0;

    int width_scale_value = 100;
    int height_scale_value = 100;
    int key = 0;

    int xOffset = 10; // Растояние между цифрами на таймлайне

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void scrollContentsBy(int dx, int dy) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

signals:
    void resizeHappend(QSize new_size);
};
#endif // PARTSHEADERGRAPHVIEW_H
