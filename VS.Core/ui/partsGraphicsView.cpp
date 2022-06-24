#include "partsGraphicsView.h"

PartsGraphicsView::PartsGraphicsView(QWidget *parent)
{
    setScene(new QGraphicsScene());    
    setMouseTracking(true);
    setViewportUpdateMode(MinimalViewportUpdate);
    setCacheMode(CacheBackground);
    setOptimizationFlag(IndirectPainting);
    // Устанавливается диапазон для ползунков
    width_scale->setRange(-100, 100);
    height_scale->setRange(-100, 100);

    // Создаётся каретка времени и её линия
    // Создаётся линия
    line = new QLine(0, 0, 0, height()*100);
    // Создаётся графический элемент сцены на её основе
    lnk = new QGraphicsLineItem(*line);
    // устанавливается цвет и толщина
    lnk->setPen(QPen(Qt::black, 1));
    // Устанавливается позиция
    lnk->setPos(0, 0);
    // и запрет на трансформацию
    lnk->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    // Добавляется в сцену ссылка на линию от каретки времени
    scene()->addItem(lnk);
}

void PartsGraphicsView::sliderChange(int eventWidth, int eventHeight)
{
    // Горизонтальная полоса
    //Размер полосы прокрутки 80% длинны нотного стана
    horizontalScrollBar()->setMaximumWidth(eventWidth * 0.8);
    horizontalScrollBar()->setMinimumWidth(eventWidth * 0.8);

    //Ползунок 17,5% от длинны нотного стана
    width_scale->setMaximumWidth(eventWidth * 0.175);
    width_scale->setMinimumWidth(eventWidth * 0.175);

    //Сдвигаем ползунок к полосе прокрутки
    width_scale->move(QPoint(eventWidth * 0.8 * 0.025 + eventWidth * 0.8, eventHeight));

    // Вертикальная полоса
    //Размер полосы прокрутки 70% длинны нотного стана
    verticalScrollBar()->setMaximumHeight(eventHeight * 0.7);
    verticalScrollBar()->setMinimumHeight(eventHeight * 0.7);

    //Ползунок 27,5% от длинны нотного стана
    height_scale->setMaximumHeight(eventHeight * 0.27);
    height_scale->setMinimumHeight(eventHeight * 0.27);

    //Сдвигаем ползунок к полосе прокрутки
    height_scale->move(QPoint(eventWidth, eventHeight - eventHeight * 0.27));
}

void PartsGraphicsView::createPart(QString type, QString path){
    // Создаём голову партии и тело
    head = new PartHead(nullptr, type);
    body = new PartBody(head, type);

    if (type == "vocal"){
        QObject::connect(body, &PartBody::changeScene,
                         PR_sound, &PianoRollGraphicsView::setNewScene);
    }
    head->bodies.append(body);

    // Компануем элементы на голове
    head->headPlacement(_width, _height);
    // опускаем части созданые позже
    head->move(0, _height * heads.count() + _height * heads.count()/10);
    // Компануем тело
    body->resize(_width, _height);
    // перемещаем вместе с головой
    body->setPos(0, _height * heads.count() + _height * heads.count()/10);
    // Устанавливает громкость в половину
    head->vol->setValue(100);

    // Если партия инструментальная, то создаём буфер для звука
    if (type == "audio"){
        // Передаётся путь для выбранного файла в класс
        audio = new Audio(); //"assets/instruments/Cruella_R.wav"
        // Если файл не подойдёт, то вызывается оповещение
        if(!audio->openFile(path.toStdString())){
            qDebug() << "Не удалось загрузить файл:" << path;
            QMessageBox::warning(this, tr("Ошибка загрузки файла"),
                                 tr("Не удалось загрузить файл."), QMessageBox::Ok);
            return;
        }
        // Передаются ссылки на аудио данные
        head->setAudio(audio);
        body->setAudio(audio);

        // тело партии масштабируется в зависимости от длинны аудио
        body->resize(audio->duration() * 1000 * scene()->width()/gMaxTime, _height);

        if (audio->duration() * 1000 > gMaxTime){
            gMaxTime = audio->duration() * 1000 + 60 * 1000;
        }
        QObject::connect(head->vol, SIGNAL(valueChanged(int)), audio, SLOT(setVolume(int)));
        QObject::connect(head->direction, SIGNAL(valueChanged(int)), audio, SLOT(setMove(int)));
        // Если данные изменятся в классе с аудио, то гистограмма обновляется
        QObject::connect(audio, &Audio::dataUpdeted, body, [=](){
            if (body && body->audio()){
                if (body->audio()->numChannels() > 1){
                    body->setRSamples(body->audio()->RDecibels());
                    body->setLSamples(body->audio()->LDecibels());
                }
                else{
                    body->setSamples(body->audio()->LDecibels());
                }
            }
        });
        // Отправляется сигнал для отрисовки гистограммы
        audio->dataUpdeted();
        //В качестве имени партитуры устанавливается имя файла
        //      C:/path/hoho.wav --> hoho
        head->setName(path.mid(path.lastIndexOf("/") + 1,
                               path.lastIndexOf(".") - 1).toStdString());
    }

    // Подключаются функции выделения
    QObject::connect(body, SIGNAL(select(PartBody*)), this, SLOT(selected(PartBody*)));
    QObject::connect(head, SIGNAL(select(PartHead*)), this, SLOT(selected(PartHead*)));
}

void PartsGraphicsView::addPart(QString type, QString path){
    // создаём партию
    createPart(type, path);

    // Добавление созданых элементов в сцены
    header->scene()->addWidget(head);
    scene()->addItem(body);

    // добавляем партию в список партий
    heads.append(head);
    resizeView(QSize(heads.first()->width(),
                     heads.first()->height()));
}

void PartsGraphicsView::setTimelineHeader(PartsTimelineGraphicsView *_timeline,
                                          PartsHeaderGraphicsView *_header)
{
    timeline = _timeline;
    header = _header;
    scene()->setSceneRect(0, 0, timeline->scene()->width(), timeline->scene()->height());
    QObject::connect(header, SIGNAL(resizeHappend(QSize)), this, SLOT(resizeView(QSize)));
    // Подключаются ползунки масштаба
    scaleConnect();
}

void PartsGraphicsView::scaleConnect(){
    setTransformationAnchor(QGraphicsView::NoAnchor);
    timeline->setTransformationAnchor(QGraphicsView::NoAnchor);
    header->setTransformationAnchor(QGraphicsView::NoAnchor);
    QObject::connect(width_scale,  &QSlider::valueChanged, this,
                     [=](){
                         _matrix.reset();
                         _width_scale = qPow(_zoom_factor, width_scale->value());
                         _matrix.translate(timeline->indicator->pos().x(), 0);
                         _matrix.scale(_width_scale, _height_scale);
                         _matrix.translate(-timeline->indicator->pos().x(), 0);
                         setTransform(_matrix);
                         timeline->setTransform(_matrix);
                     });
    QObject::connect(height_scale,  &QSlider::valueChanged, this,
                     [=](){
                         _matrix.reset();
                         _height_scale = qPow(_zoom_factor, height_scale->value());
                         setTransform(_matrix.scale(_width_scale, _height_scale));
                         _matrix.reset();
                         header->setTransform(_matrix.scale(1, _height_scale));
                     });
    // выставляем значение ползунков
    width_scale->setValue(-50);
    height_scale->setValue(-10);
}

void PartsGraphicsView::select(PartBody *body, PartHead *head)
{
    // Проверка на нажатие на пуcтую область
    if (head == nullptr && body == nullptr){
        for (int i = 0; i < heads.count(); i++) {
            heads.at(i)->setSelectAll(false);
        }
    }
    // Проверка на нажатие на "голову" партии
    else if (head){
        // Если CTRL не нажат
        if (header->key != Qt::Key_Control){
            // снимаются все выделения
            for (int i = 0; heads.count() > i; i++) {
                heads.at(i)->setSelectAll(false);
            }
            // выделяется последняя нажатая "голова"
            head->setSelectAll(true);
        }
        // Если был нажат CTRL
        else{
            // и голова была выделены, то выделение снимается
            if (head->selected())
                head->setSelectAll(false);
            // но голова не была выделена, то выделяется
            else
                head->setSelectAll(true);

        }
    }
    // Проверка на нажатие на "тело" партии
    else if (body){
        head = qobject_cast<PartHead*>(body->head);
        // Нажатие с зажатым CTRL
        if (_key == Qt::Key_Control){
            // Если элемент выделенен, то снимаем выделение
            if (body->selected()){
                body->setSelect(false);
                if(!head->isSomeSelected()){
                    head->setSelect(false);
                }
                return;
            }
            // Если элемент не выделен, то выделяем
            body->setSelect(true);
        }
        // Если нажатие пришлось на "тело" без CTRL
        else{
            // Снимаются все выделения
            for (int i = 0; i < heads.count(); i++) {
                heads.at(i)->setSelectAll(false);
            }
            // Выделяется последняя нажатая партия
            body->setSelect(true);
            head->setSelect(true);
        }
    }
}

void PartsGraphicsView::mainPlay(){
//    for (int var = 0; var < heads.count(); var++) {
//        heads.at(var)->audio()->play();
//    }
}

void PartsGraphicsView::mainSetTime(int indPos){
    // Высчитывается новое время по положению каретки
    gTime = (gMaxTime/scene()->width()) * (indPos+5);
    PR_timeline->indicator->setPos(indPos, 0);
    timeline->indicator->setPos(indPos, 0);
    checkLineCollisions(indPos);
}

void PartsGraphicsView::checkLineCollisions(int indPos){
    QList<QGraphicsItem*> list = scene()->collidingItems(timeline->indicator->lnk);
    auto a1 = std::async(std::launch::deferred, [=]{
        for (int i = 0; i < list.count(); ++i) {
            // Находится "тело" партии
            body = qgraphicsitem_cast<PartBody*>(list.at(i));
            if(!body) {continue;}
            // Вычислется новое время для конкретного трека
            float newTime = (indPos - (body->pos()).x()) * (gMaxTime/1000/scene()->width());

            // для конкретной партии устанавливается время, с которого происходит проигрывание
            if (body->audio()) {
//                body->audio()->seekTo(newTime);
                // если нажата пауза, то останавливается проигрывание
                if (!Project::PLAY_STATE){ body->audio()->pause(); }
                 // Если пауза не нажата и нота не проигрывалась, то запускается
                if (Project::PLAY_STATE && !body->audio()->isPlaying()){ body->audio()->play(); }
                // для ноты устанавливается время, с которого происходит проигрывание
                else if (!body->audio()->isPlaying()){ body->audio()->seekTo(newTime); }
             }
            else if (body->part() == "vocal") {
                body->PRscene()->checkLineCollisions(PR_timeline->indicator->lnk,
                                                         Project::PLAY_STATE);
             }
        }
    });
    a1.get();
}

// Функция удаления партии и перемещения оставшихся выше
void PartsGraphicsView::delPart()
{
    for (int i = 0; heads.count() > i; ++i)
    {
        if (heads[i]->selected()) // Если голова выделена
        {
            for (int y = 0; heads[i]->bodies.count() > y; ++y) // Удаляются все тела из сцены
            {
                scene()->removeItem(heads[i]->bodies.at(y));
                if(PR_sound->PR_scene == heads[i]->bodies.at(y)->PRscene()){
                    PR_sound->setNewScene();
                }
                heads[i]->bodies.at(y)->PRscene()->destroyed();
                delete heads[i]->bodies.at(y);
            }
            delete heads[i]; // Удаляется голова данной партии
            heads.removeAt(i); // Данная голова удаляется из списка голов
            for (int y = i; heads.count() > y; ++y)
            {
                qreal oldY = heads[y]->y();
                qreal newY = heads[y]->y() - _height - (_height/10 * 2);
                if (i < heads.count() - 2) { newY += _height/10;  }
                // Создаётся группа с анимациями
                QParallelAnimationGroup * animationGroup = new QParallelAnimationGroup (this);
                QPropertyAnimation* animation = new QPropertyAnimation(heads[y], "pos");
                animation->setEasingCurve(QEasingCurve::InOutSine);
                animation->setDuration(animationPeriodMS);
                animation->setStartValue(heads[y]->pos());
                animation->setEndValue(QPoint(heads[y]->x(), newY));
                animationGroup->addAnimation(animation); // Анимация добавляется в группу
                // добавление анимации телам партитур
                foreach(PartBody* var, heads[y]->bodies)
                {
                    animation = new QPropertyAnimation(var, "y");
                    animation->setEasingCurve(QEasingCurve::InOutSine);
                    animation->setDuration(animationPeriodMS);
                    animation->setStartValue(oldY);
                    animation->setEndValue(newY);
                    animationGroup->addAnimation(animation);
                }
                // Анимация запускается
                animationGroup->start();
            }
        }
    }
}

void PartsGraphicsView::onAnimationFinished() {
    sender()->deleteLater();
}

void PartsGraphicsView::resizeView(QSize size){
    for (int i = 0; heads.count() > i; i++) {
        heads[i]->headPlacement(size.width(), _height);
    }
    _width = size.width();
}

void PartsGraphicsView::mousePressEvent(QMouseEvent *event)
{
    setPressed(true);
    oldMousePos = mapToScene(event->pos().x(), event->pos().y()).toPoint();

    // Проводится поиск тел партии в редакторе
    body = qgraphicsitem_cast<PartBody*>(scene()->itemAt(QPointF(oldMousePos), QTransform()));

    // Если тело есть, то получается ссылка на голову
    if (body){
        // Если нажата ПКМ, то открывается контексное меню
        if (event->buttons() & Qt::RightButton){
            setDragMode(QGraphicsView::NoDrag);
            body->openContextMenu(event->screenPos());
        }
        head = qobject_cast<PartHead*>(body->head);
        // Передаются параметря для перемещения/масштабирования
        body->setOldPos(body->scenePos().toPoint());
        body->setOldMousePos(oldMousePos);
        body->select(body);
        body->PRscene()->setAllItemOldPos(body->pos().toPoint()); // Устанавливается позиция для перемещения
    }

    // Если туда куда нажали ничего нет
    if(!body){
        // снимаем все выделения
        select();

        // Если установлен в режим радактирования, то создаём тело партии
        if (_edit_mode){
            // Определяется позиция элемента по оси Y
            for(magnetPos = 0; magnetPos <= oldMousePos.y() - _height;){
                magnetPos += (_height + _height/10);
            }
            // Новая позиция устанавливается
            oldMousePos.setY(magnetPos);

            // Если в сцене header есть элемент на данной позиции,
            // то тело добавляется к существующей партии, сохраняя все настройки
            if (oldMousePos.y() >= 0 && oldMousePos.x() >= 0 ){
                // Устанавливается флаг создания
                setMaking(true);

                // Преобразуется элемент из сцены header в PartHead
                item = header->scene()->itemAt(QPointF(0, oldMousePos.y()), QTransform());
                pProxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
                // Если удалось преобразовать элемент
                if (pProxy){
                    head = qobject_cast<PartHead*>(pProxy->widget());
                }
                // Проверяется успех преобразования
                if (head && head->part_type() == "vocal"){
                    // ДОБАВЛЯЕМ к партии часть
                    body = new PartBody(head, "vocal");
                    body->resize(0, _height);
                    body->setPos(oldMousePos);
                    body->setOldPos(QPoint(oldMousePos.x(), oldMousePos.y()));
                    body->resize(oldMousePos);
                    body->PRscene()->setSceneX(oldMousePos.x());

                    // Добавляется новое тело в голову
                    head->bodies.append(body);
                    // Устанавливается их выделение
                    head->setSelect(true);
                    body->setSelect(true);

                    // Тело добавляется в сцену
                    scene()->addItem(body);

                    // Подключается сигнал выделения
                    QObject::connect(body, SIGNAL(select(PartBody*)),
                                     this, SLOT(selected(PartBody*)));
                    // Подключается сигнал смены сцены с нотами
                    QObject::connect(body,  SIGNAL(changeScene(PianoRollGraphicsScene*,QString)),
                                     PR_sound, SLOT(setNewScene(PianoRollGraphicsScene*,QString)));
                }
            }
        }
    }
}

void PartsGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    // Получается новая позиция относительно сцены
    newPos = mapToScene(event->pos().x(), event->pos().y()).toPoint();

    if (!_pressing)
        body = qgraphicsitem_cast<PartBody*>(scene()->itemAt(QPointF(newPos), QTransform()));

    // в режиме редактирования
    if (_edit_mode && body){
        if (_pressing){
            // Вычисляется дельта перемещения от изначального положения, до нового
            _xDiff = (newPos.x() - oldMousePos.x()) / saveGetPartIntQuantize() * saveGetPartIntQuantize();
            // Если тело масштабируется
            if (_resizing){ body->resize(newPos); }
            // Если тело только создаётся
            else if (_making){body->setLength(_xDiff); }
            // Если тело двигается
            else { body->move(newPos.x()); }
        }
        else{
            // Если получилось, то проверяется положение курсора на объекте
            // Если это положение близко к началу, то разрешается масштабирование с начала
            if (newPos.x() >= body->pos().x() - 5 &&
                newPos.x() <= body->pos().x() + 5){
                setCursor(QCursor(Qt::SizeHorCursor));
                body->setTypeOfResize(body->handleLeft);
                setResizing(true);
            }
            // Если это положение близко к концу, то разрешаем масштабирование с конца
            else if(newPos.x() >= body->pos().x() + body->length() - 5 &&
                    newPos.x() <= body->pos().x() + body->length() + 5){
                setCursor(QCursor(Qt::SizeHorCursor));
                body->setTypeOfResize(body->handleRight);
                setResizing(true);
            }
            // Если это положение в середине, то доступно только перемещение
            else{
                setCursor(QCursor(Qt::ArrowCursor));
                body->setTypeOfResize(body->handleNone);
                setResizing(false);
            }
        }
    }
    else{ setCursor(QCursor(Qt::ArrowCursor)); }
    update();
}

void PartsGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if (_making && body && body->length() < 20){
        scene()->removeItem(body);
        body->destroyed();
    }
    update();
    setMaking(false);
    setPressed(false);
    setResizing(false);
}

void PartsGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PartsGraphicsView::scrollContentsBy(int dx, int dy)
{
    timeline->horizontalScrollBar()->setValue(timeline->horizontalScrollBar()->value()-dx);
    header->verticalScrollBar()->setValue(header->verticalScrollBar()->value()-dy);
    emit scrolled();
    QGraphicsView::scrollContentsBy(dx, dy);
}

void PartsGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    emit resized();
    sliderChange(event->size().width(), event->size().height());
}

void PartsGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (_key != 0){
        qreal scaleFactor = pow(2., -event->angleDelta().x() / 240.);
        if (_width_scale > 2){
            _width_scale = 2;
            return;
        }
        else if (_width_scale < 0.5){
            _width_scale = 0.5;
            return;
        }
        if (_key == Qt::Key_Alt || _key == Qt::Key_AltGr){
            timeline->scale(1, scaleFactor);
            scale(1, scaleFactor);
        }
        else if (_key == Qt::Key_Shift){
            header->scale(scaleFactor, 1);
            scale(scaleFactor, 1);
        }
        resetCachedContent();
    }
    else
        QGraphicsView::wheelEvent(event);
}

void PartsGraphicsView::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    _key = event->key();
}

void PartsGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsView::keyReleaseEvent(event);
    _key = 0;
}

PartsGraphicsView::~PartsGraphicsView(){}
