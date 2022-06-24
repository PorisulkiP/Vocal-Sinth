#include "Indicator.h"

Indicator::Indicator()
{
    pen = QPen(Qt::black, 1); // Устанавливается обводка
    brush = QBrush(QColor(58, 117, 196)); // Устанавливается заливка
    points << QPointF(0,0)// Устанавливается геометрия картеки
           << QPointF(0,5)
           << QPointF(5,10)
           << QPointF(10,5)
           << QPointF(10,0);
    setAcceptHoverEvents(true); // Включения события наведения
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    // Смещение на 5 пикселеей, чтобы линии от картеки была по центру геометрии
    setTransform(transform().translate(-5, 0));
}

Indicator::~Indicator(){}

QRectF Indicator::boundingRect() const { return QRectF(0, 0, 10, 10); }

void Indicator::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawPolygon(points);
}

void Indicator::setLine(QLine *_line, QGraphicsLineItem *_lnk){
    *&line = _line;
    *&lnk = _lnk;
}

void Indicator::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setPressed(true);
    QGraphicsItem::mousePressEvent(event);
}

void Indicator::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void Indicator::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    setPressed(false);
}

void Indicator::hoverEnterEvent(QGraphicsSceneHoverEvent *event){}

void Indicator::hoverMoveEvent(QGraphicsSceneHoverEvent *event){}

void Indicator::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){}

QVariant Indicator::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // Значения новой позиции
        newPos = value.toPointF();
        newPos.setY(y());
        if(newPos.x() < 0)
            newPos.setX(0);
        lnk->setPos(newPos.x(), line->p1().y());
        update();
        return QPointF(newPos.x(), 15);
    }
    return QGraphicsItem::itemChange(change, value);
}
