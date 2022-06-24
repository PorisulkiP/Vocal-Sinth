#include "partsTimelineGraphicsView.h"

void PartsTimelineGraphicsView::creating()
{
    ins = xOffset/ saveGetPartStrQuantize();
    // Создаются основные элементы для работы линии времени
    for(int i = 0, sec = gMinTime, min = 0; i <= gMaxTime / 1000; i++, sec++){
        // Высчитывается значение времени для его дальнейшего вытавления
        if (sec == 60){
            min++;
            sec = 0;
        }
        // Создание числа
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
        for (int var = 1; var < saveGetPartStrQuantize(); ++var) {
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
    // Каретка добавляется в сцену
    scene()->addItem(indicator);
    // Создаётся таймер для перемещения каретки
    createTimer();
}

void PartsTimelineGraphicsView::createTimer() {
    int updateFRQ = 25;
    // устанавливается интервал для таймера
    pTimer->setInterval(updateFRQ);

    // speed = dist / time
//    float BPM = saveGetTempo() / 60.;
//    float normalSpeed = scene()->width()/gMaxTime;
    gPxInMs = scene()->width() / (gMaxTime / 1000);
//    float updatePosInterval = normalSpeed * BPM;

    // Получается позиция каретки
    newXPos = indicator->pos().x();

    // Устанавливается связь окончания работы таймера
    // с окончанием времени на линии времени
    connect(pTimer, &QTimer::timeout, [this](){
        if(gTime < gMaxTime) {
            float BPM = saveGetTempo() / 60.;
            float normalSpeed = scene()->width()/gMaxTime * 10;

            float updatePosInterval = BPM * normalSpeed;
//            qDebug() << "Parts BPM:" << BPM;
//            qDebug() << "Parts normalSpeed:" << normalSpeed;
//            qDebug() << "Parts updatePosInterval:" << updatePosInterval;

            // Вычисляем новое положение каретки
            newXPos += updatePosInterval;
            // Устанавливается новая позиция
            indicator->setPos(newXPos, 0);
            // Добавляется милисекунда в глобальное время
            emit newTime(newXPos);
        }
        else{
            // Таймер  останавливается
            pTimer->stop();
        }
    });
}

void PartsTimelineGraphicsView::startAnimate() {
    /* Осуществляется проверка на соответсвие новой позиции предыдущей
    /* если позиция отличается, то выставляем её в переменную newXPos
    /* для дальнейшего перемещения каретки без перезапуска таймера */
    if (remaining != indicator->pos().x())
        newXPos = indicator->pos().x();
    // Запускается таймер
    pTimer->start();
}

void PartsTimelineGraphicsView::stopAnimate()
{
    // Выставляется значение прошлой позиции
    remaining = indicator->pos().x();
    // Останавливаем таймер
    pTimer->stop();
}

void PartsTimelineGraphicsView::mousePressEvent(QMouseEvent *event){
    QGraphicsView::mousePressEvent(event);    
    if (event->button() == Qt::LeftButton){
        // Устанавливается позиция картеки времени
        if (mapToScene(event->pos().x(), 0).toPoint().x() <= 0)
            indicator->setPos(mapToScene(QPoint(0, 0)));
        else
            indicator->setPos(mapToScene(QPoint(event->pos().x(), 0)));
        // Получается ныняшняя позиция курсора мыши
        oldMousePos =  mapToScene(event->pos().x(), event->pos().y()).toPoint();
        // Устанавливается ныняшняя позиция каретки
        oldPos = QPoint(indicator->pos().x(), 0);
        // Устанавливается флаг нажатия на линию времени
        setPressed(true);
        // Отправляется сигнал о новом времени
        emit newTime(indicator->pos().x());
        emit stopPlaying();
    }
    else if(event->button() == Qt::RightButton){
        // открывается меню с настроками линии времени
    }
}

void PartsTimelineGraphicsView::mouseMoveEvent(QMouseEvent *event){
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

void PartsTimelineGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    QGraphicsView::mouseReleaseEvent(event);
    // Выпускается сигнал о установке нового времени
    emit newTime(indicator->pos().x());
    // Снимается флаг нажатия на линию времени
    setPressed(false);
}

void PartsTimelineGraphicsView::mouseDoubleClickEvent(QMouseEvent *event){
    QGraphicsView::mouseDoubleClickEvent(event);
}

PartsTimelineGraphicsView::~PartsTimelineGraphicsView(){}
