#include "partsHeaderGraphicsView.h"

void PartsHeaderGraphicsView::mousePressEvent(QMouseEvent *event){
    QGraphicsView::mousePressEvent(event);
}

void PartsHeaderGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    QGraphicsView::mouseReleaseEvent(event);
}

void PartsHeaderGraphicsView::mouseMoveEvent(QMouseEvent *event){
    QGraphicsView::mouseMoveEvent(event);
}

void PartsHeaderGraphicsView::mouseDoubleClickEvent(QMouseEvent *event){
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PartsHeaderGraphicsView::resizeEvent(QResizeEvent *event){
    emit resizeHappend(event->size());
    scene()->setSceneRect(0, 0, event->size().width(), scene()->height());
    QGraphicsView::resizeEvent(event);
}

void PartsHeaderGraphicsView::scrollContentsBy(int dx, int dy){
    QGraphicsView::scrollContentsBy(dx, dy);
}

void PartsHeaderGraphicsView::wheelEvent(QWheelEvent *event){}

void PartsHeaderGraphicsView::keyPressEvent(QKeyEvent *event){
    QGraphicsView::keyPressEvent(event);
    key = event->key();
}

void PartsHeaderGraphicsView::keyReleaseEvent(QKeyEvent *event){
    QGraphicsView::keyReleaseEvent(event);
    key = 0;
}

PartsHeaderGraphicsView::~PartsHeaderGraphicsView(){}
