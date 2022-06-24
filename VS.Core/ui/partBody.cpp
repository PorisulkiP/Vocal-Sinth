#include "partBody.h"

PartBody::PartBody(QWidget *_parent, QString part_type){
    setCacheMode(DeviceCoordinateCache);
    head = _parent;
    _part_type = part_type;
    alGenSources(1, &_source); // Создание источника звука
    alSourcef (_source, AL_GAIN, 0.1f); // громкость

    if (_part_type == "vocal"){
        connect(_scene, &PianoRollGraphicsScene::updateNotes,
                this, &PartBody::updateNotes);
        setMainVColor();
        _scene->setSource(_source);
        _scene->setVB(VB);
        setScene(_scene);
    }
    else{ // wave
        setMainWColor();
    }
}

PartBody::~PartBody(){}

PartBody::PartBody(const PartBody &ref)
{
    // копирование данных из изначального объекта в копию`
    head = ref.head;
    _part_type = ref.part();
    _height = ref.height();
    _length = ref.length();
    _color = ref.color();
    setSelect(true);
}

/* Идея проста: рисуем картинку и отрисовываем её в представлении,
 * вместо отрисовки всех точек. */
void PartBody::drawHistogram(){
    qDebug() << "path:" << _audio->getFileName().data();
    _pixmap = new QPixmap(QSize(length(), height()));
    _pixmap->fill(Qt::transparent);
        _painter = new QPainter(_pixmap);
        _painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    _painter->setCompositionMode(QPainter::CompositionMode_DestinationOver);
    _painter->setOpacity(0.5);

    int K = 10; // Оптимизатор, чем больше, тем больше пропуска образцов
    float interval = _LSamples.count() / (length());

    if (_audio->numChannels() == 1){
        futureDrawHistogram = std::async(std::launch::async, [=]{
            std::unique_lock<std::mutex> lock(_mutex);
            for (int var = 0; var < _LSamples.count() - 1; var += K) {
                _painter->drawLine(QPointF(var / interval, 10 * _LSamples.at(var) + height()/2),
                                   QPointF(var / interval, 10 * _LSamples.at(var + 1) + height()/2));
            }
        });
    }
    else {
        futureDrawHistogram = std::async(std::launch::async, [=]{
            for (int var = 0; var < _LSamples.count() - 1; ++var) {
                _mutex.lock();
                _painter->drawLine(QPointF(var / interval / K,
                                           5 * _LSamples.at(var) + height()*3./4),
                                   QPointF(var / interval / K,
                                           5 * _LSamples.at(var + 1) + height()*3./4));
                _mutex.unlock();
            }
            for (int var = 0; var < _RSamples.count() - 1; ++var) {
                _mutex.lock();
                _painter->drawLine(QPointF(var / interval / K,
                                           5 * _RSamples.at(var) + height()*1./4),
                                   QPointF(var / interval / K,
                                           5 * _RSamples.at(var + 1) + height()*1./4));
                _mutex.unlock();
            }
        });
    }
    update();
}

void PartBody::drawNotes(){
    _pixmap = new QPixmap(_length, _height);
    _pixmap->fill(Qt::transparent);
    _painter = new QPainter(_pixmap);
    _painter->setPen(QPen(Qt::white, 10, Qt::SolidLine, Qt::FlatCap));
    _painter->setCompositionMode(QPainter::CompositionMode_Plus);

    QList<QGraphicsItem*> list = PRscene()->items();
    notesDrawHistogram = std::async(std::launch::async, [=]{
        for (const auto &item : list) {
            auto track = qgraphicsitem_cast<Track*>(item);
            if (!track) {continue;}
            auto y = _height / (PRscene()->height() - track->pos().y()) * 2000 - _height;
            auto x = track->pos().x() - pos().x();
            QLine line = QLine(x, y, x + track->length(), y);
            _mutex.lock();
            _painter->drawLine(line);
            _mutex.unlock();
        }
    });
    update();
}


void PartBody::resize(int width, int height) {
    setLength(width);
    setHeight(height);
}

void PartBody::resize(QPoint newPos){
    // Нота перемещается
    // Если сторона масштабирования левая
    if (_typeOfResize == handleLeft)// handleLeft = 1
    {
        // Передвигаем точну начала ноты
        setX(newPos.x()/saveGetIntQuantize() * saveGetIntQuantize());
        // Компенсируем это добавлением длинны
        setLength(_length + (_oldPos.x()/saveGetIntQuantize() * saveGetIntQuantize() - scenePos().toPoint().x()));
        // Изменяем прежню точку начала
        _oldPos.setX(scenePos().toPoint().x());
    }
    // Если сторона масштабирования правая
    else if (_typeOfResize == handleRight)// handleRight = 2
    {
        setLength((newPos.x() - _oldPos.x())/saveGetIntQuantize() * saveGetIntQuantize());
    }
    if (_length < 0) {_length = 0;}
    // Применяем изменение в boundingRect
    prepareGeometryChange();
}

void PartBody::setSelect(bool newMod)
{
    _select = newMod;

    if (_part_type == "vocal"){
        if (!_select)   { _color = baseVColor;      }
        else            { _color = selectedVColor;  }
    }
    else{ // Wave
        if (!_select)   { _color = baseWColor;      }
        else            { _color = selectedWColor;  }
    }
    update();
}

void PartBody::setAudio(Audio *a)
{
    if (a){
        _audio = a;
        _audio->setSource(_source);
    }
}

void PartBody::move(QPoint newPos){
    _partsPos.setX((newPos + (_oldPos - _oldMousePos)).x());
    _partsPos.setY((newPos + (_oldPos - _oldMousePos)).y());

    auto yDiff = newPos.y() - _oldMousePos.y();

    if (_partsPos.x() >= 0 && _partsPos.y() >= 0){
        if (abs(yDiff) > _height){
            newPos.setY(_oldPos.y() + yDiff/(_height + 6) * (_height + 6));
        }
        else{
            newPos.setY(_oldPos.y());
        }
        if (!_making) { // в процессе создания не двигается трек по оси х
            newPos.setX(_partsPos.x());
        }
        setPos(newPos);
    }
    else if (_partsPos.y() < 0 && _partsPos.x() > 0){
        newPos.setX((newPos + (_oldPos - _oldMousePos)).x());
        setPos(newPos.x(), 0);        
    }
    else if (_partsPos.y() >= 0  && _partsPos.x() < 0){
        if (abs(yDiff) > _height){
            newPos.setY(_oldPos.y() + yDiff/(_height + 6) * (_height + 6));
        }
        setPos(0, newPos.y());
    }
    else{setPos(0, 0);}
//    qDebug() << "pos().x():" << pos().x();
    // После перемещения двигается сцена и все элементы в ней
    if(part() == "vocal"){
        PRscene()->moveAllItem(pos().x());
        PRscene()->setScenePos(pos());
    }
}

void PartBody::setLength(qreal width){
    _length = width;
    if (_scene){
        // Масштабируется сцена, привязанная к телу
        _scene->setSceneW(width);
    }
}

void PartBody::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option,
                  QWidget *widget){
    painter->setBrush(QBrush(_color));
    painter->setPen(QPen(QBrush(_color), 1));
    painter->drawRoundedRect(boundingRect(), 0, 0);
    bool isReady = false;
    try {
        if (_part_type == "vocal"){
            isReady = notesDrawHistogram.wait_for(std::chrono::seconds(1)) == std::future_status::ready;
        }
        else{
            isReady = futureDrawHistogram.wait_for(std::chrono::seconds(1)) == std::future_status::ready;
        }
    }
    catch (std::future_error const & err) {return;}
    if (isReady){
        painter->drawPixmap(0, 0, *_pixmap);
    }
}

void PartBody::openContextMenu(QPoint screenPos)
{
    emit select(this);
    QMenu *menu = new QMenu();
    //
    QAction edit("Edit Sourse", this);
    QAction clearCache("Clear cache", this);
    //
    QAction curAct("Cut", this);
    QAction copyAct("Copy", this);
    QAction pastAct("Past", this);
    QAction delAct("Delete", this);
    //
    QAction setAudioAct("Set Audio", this);
    QAction splitAct("Split", this);
    QAction mergeAct("Merge", this);
    //
    QAction renameAct("Rename", this);
    QAction muteAct("Mute", this);

    // ДЛЯ ЗАЩИТЫ ВКР
    connect(&setAudioAct, &QAction::triggered, [this](){
        QString path = QFileDialog::getOpenFileName(qobject_cast<QWidget*>(this),
                                                    tr("Выберите wav файл:"),
                                                    QDir::currentPath(),
                                                    tr("Wave (*.wav);;"));
        _audio = new Audio(path.toStdString());
    });

    menu->addAction(&edit);
    menu->addAction(&clearCache);
    menu->addSeparator();
    menu->addAction(&curAct);
    menu->addAction(&copyAct);
    menu->addAction(&pastAct);
    menu->addAction(&delAct);
    menu->addSeparator();
    menu->addAction(&setAudioAct);
    menu->addAction(&splitAct);
    menu->addAction(&mergeAct);
    menu->addSeparator();
    menu->addAction(&renameAct);
    menu->addAction(&muteAct);

    menu->exec(screenPos);
}

void PartBody::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    emit select(this);
}

void PartBody::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsItem::mouseMoveEvent(event);
}

void PartBody::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    if (_scene){
        // Отправляется сигнал на смену сцены
        emit changeScene(_scene, _scene->partName());
    }
}

