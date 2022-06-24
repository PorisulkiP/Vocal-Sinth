#ifndef INDICATOR_H
#define INDICATOR_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QVector>
#include <QPointF>
#include <QBrush>
#include <QLine>
#include <QPen>

class Indicator: public QGraphicsObject
{
    Q_OBJECT
public:
    Indicator();
    ~Indicator();

    // Устанавливается линии для картеки времени
    void setLine(QLine *_line, QGraphicsLineItem *_lnk);

    // Линия для каретки времени
    QLine *line = nullptr;
    // Ссылка на эту же линию, но в сцене с нотами/партиями
    QGraphicsLineItem *lnk = nullptr;

    void setPressed(bool newMode){ m_pressed = newMode; }
    bool pressed(){ return m_pressed; }

private:
    bool m_pressed = false; // Флаг нажата ли каретка

    QVector<QPointF> points; // Точки с головы каретки
    QPointF newPos; // Новая позиция картеки
    QBrush brush;   // Заливка каретки
    QPen pen;       // Обводка каретки

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

};

#endif // INDICATOR_H
