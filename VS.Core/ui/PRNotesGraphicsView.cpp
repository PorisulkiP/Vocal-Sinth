#include "PRNotesGraphicsView.h"

PRNotesGraphicsView::PRNotesGraphicsView(QWidget *parent){
    // Создаётся сцена для нот
    setScene(new QGraphicsScene());
}

void PRNotesGraphicsView::mousePressEvent(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
}

void PRNotesGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
}

void PRNotesGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);
}

void PRNotesGraphicsView::mouseDoubleClickEvent(QMouseEvent *event) {
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PRNotesGraphicsView::resizeEvent(QResizeEvent *event) {
    // Отправляется сигнал на изменение длинны нот
    emit resized(event->size().width());
    QGraphicsView::resizeEvent(event);
}

void PRNotesGraphicsView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
}

void PRNotesGraphicsView::wheelEvent(QWheelEvent *e) {
    // Нельзя прокурчивать отдельно от pianoRoll
    // QGraphicsView::wheelEvent(e);
}
