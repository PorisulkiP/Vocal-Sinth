#include "PRGraphicsView.h"

PianoRollGraphicsView::PianoRollGraphicsView(QWidget *parent)
{
    setPRScene(new PianoRollGraphicsScene());
    setCacheMode(CacheBackground);
    setMouseTracking(true);
    setOptimizationFlag(IndirectPainting);
    setViewportUpdateMode(MinimalViewportUpdate);

    // Устанавливается диапазон для ползунков
    width_scale->setRange(-100, 200);
    height_scale->setRange(-50, 50);
}

void PianoRollGraphicsView::sliderChange(int eventWidth, int eventHeight)
{
                      // Горизонтальная полоса
    // Размер полосы прокрутки 80% длинны нотного стана
    horizontalScrollBar()->setMaximumWidth(eventWidth * 0.8);
    horizontalScrollBar()->setMinimumWidth(eventWidth * 0.8);

    // Ползунок 17,5% от длинны нотного стана
    width_scale->setMaximumWidth(eventWidth * 0.175);
    width_scale->setMinimumWidth(eventWidth * 0.175);

    // Сдвигается ползунок к полосе прокрутки
    width_scale->move(QPoint(eventWidth * 0.8 * 0.025 + eventWidth * 0.8, eventHeight));

                      // Вертикальная полоса
    // Размер полосы прокрутки 70% длинны нотного стана
    verticalScrollBar()->setMaximumHeight(eventHeight * 0.7);
    verticalScrollBar()->setMinimumHeight(eventHeight * 0.7);

    // Ползунок 27,5% от длинны нотного стана
    height_scale->setMaximumHeight(eventHeight * 0.27);
    height_scale->setMinimumHeight(eventHeight * 0.27);

    // Сдвигается ползунок к полосе прокрутки
    height_scale->move(QPoint(eventWidth, eventHeight - eventHeight * 0.27));
}

void PianoRollGraphicsView::scaleConnect(){
    // Подключение сигналов
    /* Когда ползунок слайдера ширины в PianoRollGraphicsView двигается
     * проверяется масштаб сцены и изменяем его, если ограничели позволяют*/
    setTransformationAnchor(QGraphicsView::NoAnchor);
    timeline->setTransformationAnchor(QGraphicsView::NoAnchor);
    notes->setTransformationAnchor(QGraphicsView::NoAnchor);
    QObject::connect(width_scale,  &QSlider::valueChanged, this,
                     [=](){
                         _matrix.reset();
                         _width_scale = qPow(_zoo_factor, width_scale->value());
                         _matrix.translate(timeline->indicator->pos().x(), 0);
                         _matrix.scale(_width_scale, _height_scale);
                         _matrix.translate(-timeline->indicator->pos().x(), 0);
                         setTransform(_matrix);
                         timeline->setTransform(_matrix);
                     });
    QObject::connect(height_scale,  &QSlider::valueChanged, this,
                     [=](){
                         _matrix.reset();
                         _height_scale = qPow(_zoo_factor, height_scale->value());
                         setTransform(_matrix.scale(_width_scale, _height_scale));
                         _matrix.reset();
                         notes->setTransform(_matrix.scale(1, _height_scale));
                     });
    // Выставляются значение ползунков
    width_scale->setValue(50);
    height_scale->setValue(-10);
}

void PianoRollGraphicsView::setAll(PRTimelineGraphicsView *_timeline, PRNotesGraphicsView *_notes){
    timeline = _timeline;
    notes = _notes;

    timeline->indicator->setLine(PR_scene->line, PR_scene->lnk);
    setSceneH(notes->sceneRect().height());
    scaleConnect();
}

void PianoRollGraphicsView::setPRScene(PianoRollGraphicsScene *_scene){
    if (!_scene) { PR_scene = new PianoRollGraphicsScene(); }
    else { PR_scene = _scene;  }
    if (timeline){
        // Линия времени из новой сцены подключается к каретке
        timeline->indicator->setLine(PR_scene->line, PR_scene->lnk);
        // Подключается слежение линии времени за позицией трека со сценой
        QObject::connect(PR_scene, &PianoRollGraphicsScene::sceneRectChanged,
                         this, [=](){
                             setSceneRect(PR_scene->sceneRect());
                             if (PR_scene->sceneRect().width() > 100){
                                setSceneW(PR_scene->sceneRect().width() - 100);
                             }
                             else{
                                 setSceneW(PR_scene->sceneRect().width());
                             }

                             // получаются все объекты сцены
                             QList<QGraphicsItem*> list = scene()->items();
                             auto a1 = std::async(std::launch::async, [=]{
                                 for (const auto &item : list) {
                                     auto track = qgraphicsitem_cast<Track*>(item);
                                     if (!track) {return;}
                                     track->setPos(track->pos().x() + newPos.x(), track->pos().y());
                                 }
                             });
                             a1.get();

                             timeline->setSceneX(PR_scene->sceneRect().x());
                             timeline->setSceneW(PR_scene->sceneRect().width());
                             timeline-> horizontalScrollBar()->setValue(
                                 timeline->horizontalScrollBar()->value()-PR_scene->sceneRect().x()
                                 );
                             horizontalScrollBar()->setValue(
                                 horizontalScrollBar()->value()-PR_scene->sceneRect().x()
                                 );
                             });
    }
    // Устанавливается новая сцена
    setScene(PR_scene);
    emit newPartName(PR_scene->partName());

    // Изменяется область сцены для корректировки в представлении
    PR_scene->sceneRectChanged(PR_scene->sceneRect());

    if (_syllableText)
        _syllableText->setCompleter(PR_scene->completer());
}

void PianoRollGraphicsView::setNewScene(PianoRollGraphicsScene *_scene, QString _partName){
    setPRScene(_scene);
    if (_partName != ""){
        _scene->setPartName(_partName);
    }
}

void PianoRollGraphicsView::setSyllableText(SyllableEdit *syllableText)
{
    _syllableText = syllableText;
    QObject::connect(_syllableText, &SyllableEdit::editingFinished, this, [=](){
        // Получаются все выделенные ноты
        QList<QGraphicsItem *> all = items();
        foreach (auto var, all) {
            if(var->type() == Track::Type)
            {
                auto *i = qgraphicsitem_cast<Track*>(var);
                if (i->isSelected())
                {
                    // Нотам устанавливается новый слог
//                    i->setSyllable("[" + _syllableText->syllable() + "]");
                    i->setLyrics(_syllableText->lyrics());
                    update();
                }
            }
        }
    });
}

void PianoRollGraphicsView::setLyricsText(SyllableEdit *LE)
{
    QObject::connect(LE, &SyllableEdit::editingFinished, this, [=](){
        // Получаются все выделенные ноты
        QList<QGraphicsItem *> all = items();
        foreach (auto var, all) {
            if(var->type() == Track::Type)
            {
                auto *i = qgraphicsitem_cast<Track*>(var);
                if (i->isSelected())
                {
                    // Нотам устанавливается новый слог
                    i->setLyrics(LE->lyrics());
                    update();
                }
            }
        }
    });
}

void PianoRollGraphicsView::select(Track* track){
    // Получаются все элементы сцены
    QList<QGraphicsItem *> all = items();
    // Обнуляется кол-во выделеных нот в сцене
    _selectedCount = 0;
    // Проверка на нажатие в пуcтую область
    if (track == nullptr){
        foreach (auto var, all) {
            if(var->type() == Track::Type)
            {
                auto *i = qgraphicsitem_cast<Track*>(var);
                i->setSelected(false);
            }
        }
    }
    // Проверка на нажатие на ноту
    else if (track != nullptr){
        // Проверяется кол-во выделеныйх нот
        foreach (auto var, all) {
            if(var->type() == Track::Type)
            {
                auto *i = qgraphicsitem_cast<Track*>(var);
                if (i->isSelected())
                    _selectedCount++;
            }
        }
        // Если выделено больше одной ноты
        if(_selectedCount > 1)
            // то выводится "..."
            emit selectedLyrics("...", "...");
        else{
            emit selectedLyrics(track->lyrics(), track->syllable());
        }
        // Нажатие с зажатым CTRL
        if (_key == Qt::Key_Control){
            // Если элемент выделенен, то снимаем выделение
            if (track->isSelected()){
                track->setSelected(false);
                return;
            }
            // Если элемент не выделен, то выделяем
            track->setSelected(true);
        }
        // Если нажатие пришлось на "тело" без CTRL
        else{
            // Снимаются все выделения
            foreach (auto var, all) {
                if(var->type() == Track::Type)
                {
                    auto *i = qgraphicsitem_cast<Track*>(var);
                    i->setSelected(false);
                }
            }
            // Выделяется последняя нажатая партия
            track->setSelected(true);
        }
    }
}


void PianoRollGraphicsView::createNote(int length, int delta, int duration, std::string lyric, int noteNum,
                                       int intensity, int modulation, short PBType, int overlap, int preutter,
                                       Vibrato *vibrato, std::vector<float> *envelope, std::vector<int> *PBS,
                                       std::vector<int> *PBW, std::vector<int> *PBY, std::vector<int> *PBM,
                                       int noteTempo, int XPos){
    // Создаётся нота
    track = new Track(lyric.data());
    // Установка цвета ноты
    track->setMainColor();
    // Установка всех переданных параметров
    track->setSyllable(lyric.data());
    track->setLength(length);
    track->setDelta(delta);
    track->setDuration(duration);
    track->setNoteNum(noteNum);
    track->setIntensity(intensity);
    track->setModulation(modulation);
    track->setPBType(PBType);
    track->setOverlap(overlap);
    track->setPreutter(preutter);
    track->setVibrato(vibrato);
    track->setEnvelope(envelope);
    track->setPBM(PBM);
    track->setPBS(PBS);
    track->setPBW(PBW);
    track->setPBY(PBY);
    track->setNoteTempo(noteTempo);
    track->setX(XPos);
    // Добавляется источник звука из тела партии
    track->audio()->setSource(PR_scene->source());

    // Подключение автодополнителя
    if (PR_scene->completer())
    {
        track->setCompleter(PR_scene->completer());
    }
    else
    {
        PR_scene->setCompleter();
        track->setCompleter(PR_scene->completer());
    }
    // Подключение голосового банка
    if (PR_scene->getVB())
    {
        track->setVB(PR_scene->getVB());
        track->audio()->setVB(PR_scene->getVB());
        track->audio()->setNewLength(length);
//        track->audio()->setNewTone(noteNum);
    }
    else { qDebug() << "К сцене не подключёен ГБ"; }
    // Подключается сигнал выделения треков
    QObject::connect(track, SIGNAL(select(Track*)), this, SLOT(select(Track*)));
    // Добавление ноты в сцену
    PR_scene->addItem(track);
}

void PianoRollGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->buttons() == Qt::MiddleButton){
        setLastMode(_edit_mode, _view_mode, _cursor_mode);
        setViewMode();
    }
    else {
        setPressing(true);
        oldMousePos = mapToScene(event->pos().x(), event->pos().y()).toPoint();

        // Берётся элемент из позиции, куда указал пользователь
        track = qgraphicsitem_cast<Track*>(scene()->itemAt(oldMousePos, QTransform()));
        if(track){
            track->setOldPos(track->scenePos().toPoint());
            track->setOldMousePos(oldMousePos);
        }

        // Если туда куда нажали ничего нет
        if(!track){
            // снимаем все выделения
            select();

            // Проверяется, получилось ли преобразование
            // если нет, то создаём новую слоговую ноту
            if (editMode()){
                // Находится ближайшая нота и её позиция
                for(note_num = 0, magnetPosY = startPoint.y(); magnetPosY < oldMousePos.y() - noteHeight;){
                    magnetPosY += 30;
                    note_num++;
                }
                qreal temp = (qreal)xOffset/saveGetStrQuantize();
                for(magnetPosX = startPoint.x(); magnetPosX < oldMousePos.x() - temp;){
                    magnetPosX += temp;
                }
                oldMousePos.setY(magnetPosY);
                oldMousePos.setX(magnetPosX);

                track = new Track(QString("a"), PR_scene);
                track->setVB(PR_scene->getVB()); // Устанавливается ГБ
                track->audio()->setSource(PR_scene->source()); // Устанавливается источник звука
                track->setPressing(true); // Нота нажата
                track->setResizing(true); // Нота масштабируется
                track->setCompleter(PR_scene->completer()); // Устанавливается автодополнитель
                track->setNoteNum(note_num); // Номер ноты
                track->setTypeOfResize(track->handleRight); // Тип масштабирования "справа"
                track->setPos(oldMousePos);
                track->setOldPos(QPoint(oldMousePos.x(), 0));
                track->resize(oldMousePos);
                select(track);
                QObject::connect(track, SIGNAL(select(Track*)), this, SLOT(select(Track*)));
                PR_scene->addItem(track);
            }
        }
    }
}


void PianoRollGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    // Получается позиция курсора в сцене
    QPoint pos = mapToScene(event->pos().x(), event->pos().y()).toPoint();

    if (editMode() && pressed() && track){
        if (track->resized())
            track->resize(pos);
        else
            track->moving(pos);
    }
    else if (editMode() && !pressed())
    {
        auto *item = scene()->itemAt(pos, QTransform());

        // Проверяется, получилось ли преобразование
        if (item && item->boundingRect().height() == 30)
        {
            // Элемент на позиции курсора преобразуется в Track*
            track = qgraphicsitem_cast<Track*>(item);
            // Если получилось, то проверяем положение курсора на объекте
            // Если это положение близко к началу, то разрешаем масштабирование с начала
            if (pos.x() >= track->pos().x() &&
                pos.x() <= track->pos().x() + 5){
                setCursor(QCursor(Qt::SizeHorCursor));
                track->setTypeOfResize(track->handleLeft);
                track->setResizing(true);
            }
            // Если это положение близко к концу, то разрешаем масштабирование с конца
            else if(pos.x() >= track->pos().x() + track->length() - 5 &&
                    pos.x() <= track->pos().x() + track->length() + 5 )
            {
                setCursor(QCursor(Qt::SizeHorCursor));
                track->setTypeOfResize(track->handleRight);
                track->setResizing(true);
            }
            // Если это положение в середине, то доступно только перемещение
            else
            {
                setCursor(QCursor(editCursor));
                track->setTypeOfResize(track->handleNone);
                track->setResizing(false);
            }
        }
        else
        {
            setCursor(QCursor(editCursor));
        }

    }
    else
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }
    update();
}

void PianoRollGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    // Если установлен режим выделение
    if (cursorMode()){
        // после выделение получаем все элементы в области
        QList<QGraphicsItem *> all = scene()->items(scene()->selectionArea());
        // всем элемента устанавливается флаг выделения
        foreach (auto var, all) {
            if(var->type() == Track::Type)
            {
                auto *i = qgraphicsitem_cast<Track*>(var);
                i->setSelected(true);
            }
        }
    }
    else if (editMode()){
        if (track->length() < 5){
            PR_scene->removeItem(track);
        }
        else if (!PR_scene->completer()){
            PR_scene->setCompleter();
        }
    }    
    if (track){
        if (track->resized()){
            track->audio()->setNewLength(track->length());
        }
        track->setResizing(false);
        track->setMaking(false);
        track->setPressing(false);
    }
    setPressing(false);
    update();
}

void PianoRollGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PianoRollGraphicsView::keyPressEvent(QKeyEvent *event)
{
    _key = event->key();
    // Если была нажата клавиша Delete
    if (_key == Qt::Key_Delete){

    }
    // Если была нажата клавиша Backspace
    else if (_key == Qt::Key_Delete){

    }
    QGraphicsView::keyPressEvent(event);
}

void PianoRollGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    _key = 0;
    QGraphicsView::keyReleaseEvent(event);
}

void PianoRollGraphicsView::resizeEvent(QResizeEvent *event)
{
    sliderChange(event->size().width(), event->size().height());
}

void PianoRollGraphicsView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    timeline->horizontalScrollBar()->setValue(timeline->horizontalScrollBar()->value()-dx);
    notes->verticalScrollBar()->setValue(notes->verticalScrollBar()->value()-dy);
}

void PianoRollGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (_key != 0){
        qreal scaleFactor = pow(2., -event->angleDelta().x() / 240.);
        if (_width_scale > 2){
            _width_scale = 2;
            return;
        }
        if (_width_scale < 0.5){
            _width_scale = 0.5;
            return;
        }
        if (_key == Qt::Key_Control){
                notes->scale(1, scaleFactor);
                scale(1, scaleFactor);
            }
        if (_key == Qt::Key_Shift){
                timeline->scale(scaleFactor, 1);
                scale(scaleFactor, 1);
            }
        resetCachedContent();
    }
    else
        QGraphicsView::wheelEvent(event);
}

void PianoRollGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QBrush brush(Qt::white); // Qt::darkGray
    painter->fillRect(rect,brush);
}

PianoRollGraphicsView::~PianoRollGraphicsView(){}

