#include "Track.h"

Track::Track(QString _name, QGraphicsScene *parent){
    setFlags(ItemIsMovable);
    setFlag(ItemIsSelectable);
    penWidth = 0;
    rounded = 0;
    _lyrics = _name;
    oldPos = scenePos().toPoint();
    _height = 30;
    _length = 0;
    _pressing = false;
    _making = false;
    _parent = parent;

    // Создаётся строка для ввода слога
    text = new SyllableEdit();
    // Поле скрывается
    text->hide();

    // Подключается функция после подтверждения введённых данных
    QObject::connect(text, SIGNAL(editingFinished()), this, SLOT(lyricsEditHide()));
    update();
}

void Track::lyricsEditHide(){
    _lyrics = text->lyrics();
//    _syllable = text->syllable();
    text->hide();
    if (_completer)
        _completer->popup()->hide();
    else
        qInfo() << "completer не подключён";
    scene()->update();
    notesFiender(_lyrics);
//    notesFiender(_syllable);
    emit select(this);
    emit newLyrics(_lyrics);
}

void Track::setVB(Voicebank *newVB)
{
    try {
        VB = *&newVB;
        audio()->setVB(newVB);
    } catch (...) {
        qDebug() << "Не удалось установить ГБ";
    }
    if(VB)
    {
        notesFiender(_lyrics);
    }
}

void Track::setVibrato(Vibrato *newVibrato)
{
    _vibrato = newVibrato;
}

void Track::setNoteNum(int newNoteNum)
{
    noteNum = newNoteNum - MusicMath::noteOneOct * 2 + 1;
    setNote(MusicMath::noteNameFromNum(noteNum).data());
    // MusicMath::noteOneOct * 2 это + 2 октавы, так как в
    setY((MusicMath::fullnotesCount - noteNum) * _height);
}

void Track::setNoteTempo(int newNoteTempo) {
    noteTempo = newNoteTempo;
}

void Track::setSelected(bool selected){
    _selected = selected;
    if (_selected){
        _color = mainColor.lighter(170);
    }
    else{
        _color = mainColor;
    }
    update();
}

// местоположение, длинна и высота слога
QRectF Track::boundingRect()const {
    return QRectF(0, 0, _length, _height);
}

void Track::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option,
                  QWidget *widget){
    painter->setBrush(QBrush(_color));
    painter->setPen(QPen(QBrush(Qt::black), penWidth));
    painter->drawRoundedRect(boundingRect(), rounded, rounded);

    painter->scale(1. / painter->transform().m11(),
                   1. / painter->transform().m22());
    painter->drawText(QPoint(_length / 10, _height * 4/3), _lyrics);
//    painter->drawText(QPoint(_length / 10, _height * 4/3), _lyrics + " " + _syllable);
}

void Track::note_rename(QGraphicsSceneMouseEvent *event){
    // Родительским объектом устанавливается слоговая нота
    text->setParent(event->widget());
    // Устанавливается текст
    text->setText(_lyrics);
//    text->setText(_lyrics + " " + _syllable);
    // Поле ввода масштабируется
    text->resize(QSize(100, 25));
    text->move(0, 0);
    text->move(event->scenePos().x(), _height * 3);
    text->setStyleSheet("SyllableEdit { "
                        "background-color: rgb(255, 255, 255);"
                        "border: 1px solid black; }");
    text->show();
    text->setFocus();
}

void Track::notesFiender(QString syllable){
    full_filename.clear();
    if (VB){
//        qDebug() << syllable;
        auto tmp = VB->notesFiender(syllable.toStdString());
        if (!tmp.empty()){
            string path = VB->mainOtoSet()->file();
            full_filename.append(path.erase(path.find("oto.ini")) + "\\");
            full_filename.append(tmp);
        }
        else{
            setColor(Qt::red); // если файл не найден, нота окрашивается в красный
            return;
        }
    }
    else{        
        qDebug() << "К ноте не подключёен ГБ";
        return;
    }

    // Совершается попытка открытия аудиофайла нового слога
    openAudio();
}

void Track::openAudio(){
    // Совершается попытка открытия файла
    if (!full_filename.empty()){
        if (_audio->openFile(full_filename)){
            // Если открытие прошло удачно
            brush.setColor(_color);
            return;
        }
    }
    setColor(Qt::red); // если файл не найден, нота окрашивается в красный
    qDebug() << "Can't open file:" << full_filename.data();
}

void Track::resize(QPoint newPos){
    // Нота перемещается
    moving(newPos);
    // Если сторона масштабирования левая
    if (_typeOfResize == handleLeft)// handleLeft = 1
    {
        if (x() > 0 && y() > 0)
        {
            // Передвигаем точну начала ноты
            setX(newPos.x()/saveGetIntQuantize() * saveGetIntQuantize());
            // Компенсируем это добавлением длинны
            _length = _length + (oldPos.x()/saveGetIntQuantize() * saveGetIntQuantize() - scenePos().toPoint().x());
            // Изменяем прежню точку начала
            oldPos.setX(scenePos().toPoint().x());
        }
    }
    // Если сторона масштабирования правая
    else if (_typeOfResize == handleRight)// handleRight = 2
    {
        _length = (newPos.x() - oldPos.x())/saveGetIntQuantize() * saveGetIntQuantize();
    }
    // Если длинна трека меньше 5 он будет удалён
    if (_length < 0) {_length = 0;}
    // Применяем изменение в boundingRect
    prepareGeometryChange();
}

void Track::moving(QPoint newPos){
    // Находим растояние между положениями курсора
    _yDiff = newPos.y() - oldMousePos.y();
    _xDiff = newPos.x() - oldMousePos.x();

    // Если нота не выхот за приделы по Х и Y, то перемещается по всем осям
    if (x() > 0 && y() > 0)
    {
        // Если разница растояний больше высоты, то перемещается ноту по оси Y
        if ((abs(_yDiff) >= _height) || (_yDiff == 0))
        {
            setY(oldPos.y() + _yDiff/_height * _height);
        }
        // в процессе создания не двигается трек по оси х
        if (!_resizing)
        {
            setX(oldPos.x() + _xDiff / saveGetIntQuantize() * saveGetIntQuantize());
        }
    }
    // Если нота выдейдет за пределы сцены по оси Y, то пемещается нота только по оси X
    else if (x() > 0 && y() <= 0 && !_resizing)
    {
        setPos(oldPos.x() + _xDiff / saveGetIntQuantize() * saveGetIntQuantize(), 1);
    }
    // Если нота выдейдет за пределы сцены по оси X, то пемещается будет только по оси Y
    else if (x() <= 0  && y() > 0)
    {
        // Если разница растояний больше высоты, то пемещается будет только по оси Y
        if ((abs(_yDiff) > _height) || (_yDiff == 0))
        {
            setPos(1, oldPos.y() + _yDiff/_height * _height);
        }
    }
    else{
        setPos(1,1);
    }
}

void Track::openContextMenu(QPoint screenPos)
{
    QMenu *menu = new QMenu();

    QAction play("Play", this);
    QAction clearCache("Clear cache", this);
    QAction curAct("Cut", this);
    QAction copyAct("Copy", this);
    QAction pastAct("Past", this);
    QAction delAct("Delete", this);
    //
    QAction splitAct("Split", this);
    QAction mergeAct("Merge", this);
    //
    QAction renameAct("Lyrics", this);
    QAction muteAct("Mute", this);

    menu->addAction(&play);
    menu->addAction(&clearCache);
    menu->addSeparator();
    menu->addAction(&curAct);
    menu->addAction(&copyAct);
    menu->addAction(&pastAct);
    menu->addAction(&delAct);
    menu->addSeparator();
    menu->addAction(&splitAct);
    menu->addAction(&mergeAct);
    menu->addSeparator();
    menu->addAction(&renameAct);
    menu->addAction(&muteAct);

    menu->exec(screenPos);
}

void Track::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsItem::mousePressEvent(event);
    setPressing(true);
    emit select(this);
}
void Track::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    // Вызывается функция изменения размера.
    // Если не будет флага масштабирования, то трек будет только перемещаться.
    resize(event->scenePos().toPoint());
}
void Track::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsItem::mouseReleaseEvent(event);
    setPressing(false);
}
void Track::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsItem::mouseDoubleClickEvent(event);
    note_rename(event);
}
Track::~Track(){
    delete _audio;
}

Track::operator QString() const  {
    return QString("Lyrics: " + _lyrics +
                   ", Pos: " + QString::number(startPos.x()) + ", " + QString::number(startPos.y()) +
                   ", Dur: " + QString::number(getDuration()) +
                   ", Tone: " + note() + " " + QString::number(getNoteNum()));
}

QPoint Track::getOldPos() const
{
    return oldPos;
}



