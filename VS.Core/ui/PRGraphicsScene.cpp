#include "PRGraphicsScene.h"

PianoRollGraphicsScene::PianoRollGraphicsScene(QWidget *parent)
    :QGraphicsScene(nullptr)
{
    // Количество столбцов
    for(int i = 0, x = xOffset; i <= gMaxTime / 1000; ++i){
        // Создание линии
        auto item = addLine(0, 0, 0, MusicMath::fullnotesCount * noteHeight, QPen(Qt::black, 1));
        // Установка позиции
        item->setPos(i * x, 0);
        // Установка флага на запрет трансформации
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }

    // Строчки под ноты
    for(int i = 0; i <= MusicMath::fullnotesCount; ++i){
        // Создание линии
        auto item = addLine(gMinTime, 0,
                      (gMaxTime / 1000) * xOffset, 0, QPen(Qt::black, 1));
        // Установка позиции
        item->setPos(0, i * noteHeight);
        // Установка флага на запрет трансформации
        item->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }

    setSceneH(MusicMath::fullnotesCount * noteHeight);
    // Создаётся линия картеки
    line = new QLine(0, 0, 0, height());
    // Создаётся графический элемент сцены на её основе
    lnk = new QGraphicsLineItem(*line);
    // устанавливается цвет и толщина
    lnk->setPen(QPen(Qt::black, 1));
    // Устанавливается позиция
    lnk->setPos(0, 0);
    // Установка флага на запрет трансформации
    lnk->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    // Добавляется в сцену ссылка на линию от каретки времени
    addItem(lnk);
}

PianoRollGraphicsScene::~PianoRollGraphicsScene(){
    clear();
}

void PianoRollGraphicsScene::moveAllItem(qreal x)
{
    // получаются все объекты сцены
    QList<QGraphicsItem*> list = items();
    for (const auto &item : list) {
        auto track = qgraphicsitem_cast<Track*>(item);
        if (!track) {continue;}

        track->setX(track->oldPos.x() + x);
    }
    emit updateNotes();
}

void PianoRollGraphicsScene::setAllItemOldPos(QPoint bodyPos)
{
    QList<QGraphicsItem*> list = items();
    for (const auto &item : list) {
        auto track = qgraphicsitem_cast<Track*>(item);
        if (!track) {continue;}
        track->setOldPos(track->pos().toPoint()- QPoint(bodyPos.x(), 0));
    }
}

void PianoRollGraphicsScene::checkLineCollisions(QGraphicsLineItem *_lnk, bool playState){
    // Получается списк всех элементов, что пересекаются с линией каретки врмеени
    QList<QGraphicsItem*> list = collidingItems(_lnk);
    auto future1 = std::async(std::launch::deferred, [=]{
        for (int i = 0; i < list.count(); ++i) {
            // Находится "тело" партии
            track = qgraphicsitem_cast<Track*>(list.at(i));
            if (!track) {return;}
            if (!track->audio()) {return;}
            // Вычислется новое время для конкретного трека
            // не знаю, что за 160, но с ним корректно определяется время
            // Delay = 60/(BPM*N)
            float newTime = (_lnk->pos().x() - (track->pos()).x()) * (gMaxTime/1000/width());
            if (!playState){
                // если нажата пауза, то останавливается проигрывание
                track->audio()->pause();
            }
            else if (playState && !track->audio()->isPlaying()){
                // Если пауза не нажата и нота не проигрывалась, то запускается
                track->audio()->play(); // запуск ноты
            }
            else if (!track->audio()->isPlaying()){
                // для конкретной ноты устанавливается время, с которого происходит проигрывание
                track->audio()->seekTo(newTime);
            }
        }
    });
    future1.get();
}

ALuint PianoRollGraphicsScene::source() const
{
    return _source;
}

void PianoRollGraphicsScene::setSource(ALuint newSource)
{
    _source = newSource;
}

void PianoRollGraphicsScene::setPartName(QString partName) {
    if (_partName != partName){
        _partName = partName;
        emit newPartName(partName);
    }
}

void PianoRollGraphicsScene::setVB(Voicebank *newVB)
{
     _VB = newVB;

    if (_VB){
        auto list = items();
        // Установка данного ГБ всем нотам в сцене
        for (auto var : list) {
            try{
                track = qgraphicsitem_cast<Track*>(var);
                if (track){
                    track->setVB(newVB);
                    track->audio()->setVB(newVB);
                }
            }
            catch(...){
                qDebug() << "Ошибка в " << *track;
                continue;
            }
        }
        setCompleter();
    }
}

void PianoRollGraphicsScene::setNewTempo(double tempo){

}

void PianoRollGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
}

void PianoRollGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void PianoRollGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void PianoRollGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsScene::mouseDoubleClickEvent(event);
}

void PianoRollGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent){
    QGraphicsScene::wheelEvent(wheelEvent);
}

void PianoRollGraphicsScene::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);
}

void PianoRollGraphicsScene::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyReleaseEvent(event);
}

bool PianoRollGraphicsScene::eventFilter(QObject *watched, QEvent *event)
{
    emit updateNotes();
    return PianoRollGraphicsScene::eventFilter(watched, event);
}
