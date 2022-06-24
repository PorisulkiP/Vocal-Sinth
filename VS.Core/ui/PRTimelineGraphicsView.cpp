#include "PRTimelineGraphicsView.h"

void PRTimelineGraphicsView::creating() {
    /* Время для партии высчитывается на основе положения по Х в редакторе партии
     * и длинне партии.
     * длиннаСценыПартий / максВремя = 1 рх/мс
     * положениеПартииХ * 1 рх/мс = ВремяНачала
     * (положениеПартииХ + ШиринаПартии) * 1рх/мс = ВремяОкончания
     * */

    for(int i = 0, sec = gMinTime, min = 0; i <= gMaxTime / 1000; i++, sec++) {
        // Высчитывается значение времени для его дальнейшего вытавления
        if (sec == 60) {
            min++;
            sec = 0;
        }
        // Создаётся значение числа
        if (sec < 10 && min < 10)
            item = scene()->addText(QString("0%1:0%2").arg(min).arg(sec));
        else if (sec >= 10 && min < 10)
            item = scene()->addText(QString("0%1:%2").arg(min).arg(sec));
        // Настраивается положение времени
        item->setPos(i * xOffset, 0);
        // Устанавливается флаг запрета на трансформацию
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        // Создаётся элемент секундных линий
        item = scene()->addLine(0, 25, 0, 0, QPen(Qt::black, 1));
        // Устанавливается позиция
        item->setPos(i * xOffset, 0);
        // и запрещается транформация
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);

        /* Создаются и настраиваются значение при квантовании.
        * Ниже происходит вычитание из текущей позиции следующую и деление на значение квантования
        * для определения растояние между элементами */
        ins = gPRXOffset/ saveGetStrQuantize();

        for (int var = 1; var < saveGetStrQuantize(); ++var) {
            // Создаются линии квантования
            item = scene()->addLine(0, 25, 0, 15, QPen(Qt::black, 1));
            // Устанавливается их позиция
            item->setPos(i * xOffset + ins * var, 0);
            // и запрещается транформация
            item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        }
    }
    // Устанавливается начало сцены в позиции 0,0
    scene()->setSceneRect(0,0, scene()->sceneRect().width(),
                               scene()->sceneRect().height());
    // Устанавливается положение каретки времени
    indicator->setPos(QPoint(0, 15));
    // Картека добавляется в сцену
    scene()->addItem(indicator);
    // Создаётся таймер
//    createTimer();
}

void PRTimelineGraphicsView::createTimer() {
    int updateFRQ = 25;
    pTimer->setInterval(updateFRQ); // Скорость каретки времени в миллисекундах
    // speed = dist / time
    float updatePosInterval =  (scene()->width() / gMaxTime) / (60. / (saveGetTempo() * 4));

    // Получается позиция каретки
    newXPos = indicator->pos().x();

    // Устанавливается связь окончания работы таймера
    // с окончанием времени на линии времени
    QObject::connect(pTimer, &QTimer::timeout, [=](){
        if(gTime < gMaxTime) {
            qDebug() << "updatePosInterval:" << updatePosInterval;
            // Вычисляем новое положение каретки
            newXPos += (updatePosInterval / updateFRQ);
            // Устанавливается новая позиция
            indicator->setPos(newXPos, 0);
        }
        else{
            // Таймер  останавливается
            pTimer->stop();
        }
    });
}

void PRTimelineGraphicsView::startAnimate() {
    /* Осуществляется проверка на соответсвие новой позиции предыдущей
    /* если позиция отличается, то выставляем её в переменную newXPos
    /* для дальнейшего перемещения каретки без перезапуска таймера */
    if (remaining != indicator->pos().x())
        newXPos = indicator->pos().x();
    // Запускается таймер
    pTimer->start(100);
}

void PRTimelineGraphicsView::stopAnimate() {
    // Выставляется значение прошлой позиции
    remaining = indicator->pos().x();
    // Останавливаем таймер
    pTimer->stop();
}

void PRTimelineGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        // Устанавливается позиция картеки времени
        indicator->setPos(mapToScene(QPoint(event->pos().x(), 0)));
        oldMousePos =  mapToScene(event->pos().x(), event->pos().y()).toPoint();
        oldPos = QPoint(indicator->pos().x(), 0);
        setPressed(true);
        emit stopPlaying();
        // Выпускается сигнал о установке нового времени
        emit newTime(indicator->pos().x());        
    }
    else if(event->button() == Qt::RightButton) {
        // открывается меню с настроками линии времени
    }
}

void PRTimelineGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);
    // при перемещении с зажатой ЛКМ перемещается каретка
    if (_pressed){
        // Получается новая позиция каретки
        newPos = mapToScene(event->pos().x(), event->pos().y()).toPoint();
        // Высчитывается разница с прошлой позицией
        _xDiff = newPos.x() - oldMousePos.x();
        // Устанавливается новая позиция картеки времени
        indicator->setPos(oldPos.x() + _xDiff/saveGetPartIntQuantize() * saveGetPartIntQuantize(), 0);
        // Отправляется сигнал о новом времени
        emit newTime(indicator->pos().x());
    }
}

void PRTimelineGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    // Выпускается сигнал о установке нового времени
    emit newTime(indicator->pos().x());
    // Снимается флаг нажатия на линию времени
    setPressed(false);
}

void PRTimelineGraphicsView::mouseDoubleClickEvent(QMouseEvent *event) {
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PRTimelineGraphicsView::scrollContentsBy(int dx, int dy) {
    QGraphicsView::scrollContentsBy(dx, dy);
}

PRTimelineGraphicsView::~PRTimelineGraphicsView(){}
