#ifndef PRNOTESGRAPHVIEW_H
#define PRNOTESGRAPHVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QResizeEvent>

class PRNotesGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    PRNotesGraphicsView(QWidget *parent = nullptr);
    ~PRNotesGraphicsView(){}

signals:
    void resized(int w); // Сигнал изменения размера представления

private:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void scrollContentsBy(int dx, int dy) override;
    virtual void wheelEvent(QWheelEvent *e) override;
};
#endif // PRNOTESGRAPHVIEW_H
