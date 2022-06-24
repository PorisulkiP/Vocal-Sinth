#ifndef NOTES_H
#define NOTES_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsObject>
#include <QPushButton>
#include <QVector>
#include <QString>

#include "PRNotesGraphicsView.h"
#include "musicMath.h"

/** TODO: Сделать ноты интерактивынми,
 *  чтобы при нажатии говорился слог с звучанием данной ноты */

class Notes: public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Notes(PRNotesGraphicsView *_view = nullptr);
    ~Notes(){}

public slots:
    void resizeAll(int w); // Масштабирование по ширине всех нот

private:
    QVector<QPushButton*>   notes; // Масив нот
    PRNotesGraphicsView     *view = nullptr;    // Представление нот
    QPushButton             *button = nullptr;  // кнопка под ноту

    int length = 95, height = 30;

    void nameCreating();
    void notesCreation();
    void resizeNote();

    // При нажатии воспроизводится звук данной ноты
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual QRectF boundingRect() const override { return QRectF(QRect(0, 0, length, height)); }
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {}

};

#endif // NOTES_H
