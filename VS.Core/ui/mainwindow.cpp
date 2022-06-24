#include "mainwindow.h"
#include "../ui_mainwindow.h"

// при запуске файла выполняется эта часть кода
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // TODO: Сделать функцию инициализции контекста OpenAL
    alutInit(NULL, NULL);
    alListenerf(AL_GAIN, 10); // Устанавливается громкость слушателя на 10,

    ui->setupUi(this);
    splitterConfig();

    // Создаём нотный стан из кнопок
    notes = new Notes(ui->PR_notes);

    // Подключается сцена с нотами к редактору нот
    ui->parts_body->setPRsound(ui->PR_sound, ui->PR_timeline);

    // В функцию передаются линия времения и представление с головами партий
    ui->parts_body->setTimelineHeader(ui->parts_timeline, ui->parts_head);
    // Назначается линия к каретке
    ui->parts_timeline->indicator->setLine(ui->parts_body->line, ui->parts_body->lnk);

    // Передаётся сцена, линию времени, ноты в представление нотного стана чтобы перемещать их одновременно
    ui->PR_sound->setAll(ui->PR_timeline, ui->PR_notes);

    // Устанавливается типа "Линия"
    ui->SE_symbol_edit->setType("line");
    ui->SE_lyrics_edit->setType("line");
    // Устанавливается соединение между полями ввода и нотами
    ui->PR_sound->setSyllableText(ui->SE_symbol_edit);
    ui->PR_sound->setSyllableText(ui->SE_lyrics_edit);
    ui->PR_sound->setLyricsText(ui->SE_lyrics_edit);

#ifdef REALEASE
    // Создаётся вокальная партия
    ui->parts_body->addPart("vocal");
    // Сцена партии передаётся в представление
    ui->PR_sound->setNewScene(ui->parts_body->body->PRscene(), "Main");
#endif

    // Устанавливается размер представления согласно сцене
    ui->PR_sound->setSceneH(ui->PR_sound->PR_scene->height());
    ui->PR_sound->setSceneW(ui->PR_sound->PR_scene->width());

    setViewStile();
    connectButtons(); // Подключение всех элементов интерфейса
    fillDataFromDB(); // Заполнение ГБ из БД
    change_mode(); // Изменение мода

    if (ui->parts_body->body)
    {
        ui->PR_sound->centerOn(0, 1400); // перемещение к первой октаве
    }
}

//При закрытии эта
MainWindow::~MainWindow()
{
    // Отключается контекст
    alcMakeContextCurrent(NULL);
    // Уничтожается контекст
    alcDestroyContext(context);
    // Происходит отключение от звукового девайса
    alcCloseDevice(device);
    // Отключается библиотека ALUT
    alutExit();

    delete ui;
}

void MainWindow::fillDataFromDB()
{
    QSqlDatabase *db = prj->connectToDataBase();
    QSqlQuery *query = new QSqlQuery(*db);
    query->exec("SELECT * FROM [Голосовые банки]");

    while (query->next()) {
        vocal_name = new QListWidgetItem();
        vocal_name->setText(query->value(1).toString());
        // Названия ГБ добавляются в статический массив
        Project::VBitems.emplace_back(query->value(1).toString().toStdString());
        ui->voice->addItem(vocal_name);
        if (vocal_style->text() != query->value(10).toString()){
            vocal_style = new QListWidgetItem();
            vocal_style->setText(query->value(10).toString());
            ui->voice_style_list->addItem(vocal_style);
        }
    }
    // Заполнение столбца "Языки"
    vocal_language = new QListWidgetItem();
    vocal_language->setText("Japanese");
    ui->voice_language->addItem(vocal_language);
    vocal_language = new QListWidgetItem();
    vocal_language->setText("Russian");
    ui->voice_language->addItem(vocal_language);
    vocal_language = new QListWidgetItem();
    vocal_language->setText("English");
    ui->voice_language->addItem(vocal_language);
}

//TESTS

void MainWindow::testFULL_BeautifulFarAway(){
    prj->open("assets/testPrjct/Прекрасное далёко.ust");
    auto a = new Audio("assets/testPrjct/Прекрасное далёко bit.wav");
    ui->parts_body->body->setAudio(a);
//    ui->parts_body->addPart("audio", "assets/testPrjct/Прекрасное далёко.wav");
}

void MainWindow::testFULL_Nemurihime()
{
    prj->open("assets/testPrjct/Nemurihime/ust Len.ust");
    auto a = new Audio("assets/testPrjct/Nemurihime/Len_part.wav");
    ui->parts_body->body->setAudio(a);
    ui->parts_body->body->audio()->setVolume(100);
    ui->parts_body->addPart("audio", "assets/testPrjct/Nemurihime/nemuri_inst.wav");
    ui->parts_body->head->audio()->setVolume(30);
}

void MainWindow::testFULL_CrimsonAndWhiteRedSpiderLilySong(){
//    prj->append("assets/testPrjct/testJPshort.ust");
//    prj->append("assets/testPrjct/testJPshortLen.ust");

    ui->PR_sound->centerOn(0, 1400); // перемещение к первой октаве
    prj->open("assets/testPrjct/Len_Part.ust");
    ui->parts_body->body->PRscene()->setAllItemOldPos(QPoint(0, 0));
    ui->parts_body->body->move(2500);
    prj->append("assets/testPrjct/Rin_part.ust");
    ui->parts_body->body->setAudio(a);
    ui->parts_body->body->PRscene()->setAllItemOldPos(QPoint(0, 0));
    ui->parts_body->body->move(4415);
    ui->parts_body->addPart("audio", "assets/testPrjct/OFF VOCAL Crimson and White Red Spider Lily Song.wav");
}

void MainWindow::testFULL_Shadenfroide(){
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Гакупо бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Гакупо мейн 1.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Гакупо мейн 2.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Гуми бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Гуми мейн.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Кайто бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Кайто мейн.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Лен бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Лен мейн.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Люка бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Люка мейн.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Люка мейн_2.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Мейко бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Мейко мейн.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Мику бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Мику мейн 2.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Мику мейн высокий.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Мику мейн.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Рин бэк.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/Рин мейн.wav");
    ui->parts_body->addPart("audio", "assets/testPrjct/Mr. Shadenfroide/минус_L.wav");

    // изменяем размер разделителей, полностью убирая редактор нот
    QList<int> sizes;
    sizes.append(1);
    sizes.append(1000);
    sizes.append(0);
    m_splitter->setSizes(sizes);
    sizes.clear();

    sizes.append(200);
    sizes.append(1000);
    phb_splitter->setSizes(sizes);
}

void MainWindow::testFULL_RIP()
{
    prj->open("assets/testPrjct/RIP=RELEASE/3 MAIN (CV).ust");
    ui->tempo_edit->setText(QString::number(prj->ust()->bpm()));
    ui->parts_body->addPart("audio", "assets/testPrjct/RIP=RELEASE/RIP=release off mono.wav");
}

//TESTS

////////////////////////////////////////// NOTE EDITOR //////////////////////////////////////////
// При нажатии на плей двигаем каретку
void MainWindow::on_button_play_clicked() {
    if (!Project::PLAY_STATE){
        Project::PLAY_STATE = true;
        ui->button_play->setChecked(true);
        ui->button_pause->setChecked(false);
        // Воспроизведение нот и партий происходит через checkCollision в PartsGraphicsView
        ui->PR_timeline->startAnimate();
        ui->parts_timeline->startAnimate();
    }
    else{
        on_button_pause_clicked();
    }
}

void MainWindow::on_button_pause_clicked()
{
    Project::PLAY_STATE = false;
    ui->button_play->setChecked(false);
    ui->button_pause->setChecked(true);
    for (PartHead* head : ui->parts_body->heads){
        for (PartBody* body : head->bodies){
            if(body->audio())
                body->audio()->pause();
        }
    }
    ui->PR_timeline->stopAnimate();
    ui->parts_timeline->stopAnimate();
}

void MainWindow::change_mode(){
    ui->SE_tool_pencil->setChecked(ui->PR_sound->editMode());
    ui->SE_tool_hand->setChecked(ui->PR_sound->viewMode());
    ui->SE_tool_pointer->setChecked(ui->PR_sound->cursorMode());
}

void MainWindow::on_SE_tool_pencil_clicked()
{
    ui->PR_sound->setEditMode();
    change_mode();
}

void MainWindow::on_SE_tool_pointer_clicked()
{
    ui->PR_sound->setCursorMode();
    change_mode();
}

void MainWindow::on_SE_tool_hand_clicked()
{
    ui->PR_sound->setViewMode();
    change_mode();
}

////////////////////////////////////////// END NOTE EDITOR ///////////////////////////////////////


////////////////////////////////////////// PARTS EDITOR //////////////////////////////////////////

void MainWindow::change_parts_mode()
{
    ui->parts_toolbar_pencil->setChecked(ui->parts_body->editMode());
    ui->parts_toolbar_hand->setChecked(ui->parts_body->viewMode());
    ui->parts_toolbar_pointer->setChecked(ui->parts_body->cursorMode());

    if (ui->parts_body->editMode()){
        ui->parts_body->setDragMode(QGraphicsView::NoDrag);
        ui->parts_body->setCursor(QCursor(QPixmap("assets/icons/cursor-pencel.png").scaled(20, 20)));

    }
    else if (ui->parts_body->viewMode()){
        ui->parts_body->setCursor(Qt::ArrowCursor);
        ui->parts_body->setDragMode(QGraphicsView::ScrollHandDrag);

    }
    else if (ui->parts_body->cursorMode()){
        ui->parts_body->setCursor(Qt::ArrowCursor);
        ui->parts_body->setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void MainWindow::on_parts_toolbar_pencil_clicked()
{
    ui->parts_body->setEditMode();
    change_parts_mode();
}

void MainWindow::on_parts_toolbar_hand_clicked()
{
    ui->parts_body->setViewMode();
    change_parts_mode();
}

void MainWindow::on_parts_toolbar_pointer_clicked()
{
    ui->parts_body->setCursorMode();
    change_parts_mode();
}

////////////////////////////////////////// END PARTS EDITOR //////////////////////////////////////////


void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    m_splitter->resize(this->size().width(), this->size().height() - 20);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}

void MainWindow::splitterConfig()
{
    // подключаем основной разделитель
    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->move(0, 20); // немного опускаем, чтобы не лез на меню
    m_splitter->setHandleWidth(10);

    // разделитель партий и тулбара
    p_splitter = new QSplitter(Qt::Vertical, this);
    // разделитель головы и тела партии
    phb_splitter = new QSplitter(Qt::Horizontal, this);
    // разделитель головы и пустоты времения парии
    phe_splitter = new QSplitter(Qt::Vertical, this);
    // разделитель тела и линии времения парии
    pbt_splitter = new QSplitter(Qt::Vertical, this);

    QWidget *emptyW = new QWidget(this);
    emptyW->setGeometry(0, 0, ui->parts_head->width(), ui->parts_timeline->height());
    emptyW->setFixedHeight(ui->parts_timeline->height());

    // включаем в разделитель части сцены
    m_splitter->addWidget(ui->parts_toolbar);
    m_splitter->addWidget(p_splitter);
    p_splitter->addWidget(ui->parts_timeline);
    p_splitter->addWidget(phb_splitter);
    phb_splitter->addWidget(phe_splitter);
    phe_splitter->addWidget(emptyW);
    phe_splitter->addWidget(ui->parts_head);
    phb_splitter->addWidget(pbt_splitter);
    pbt_splitter->addWidget(ui->parts_timeline);
    pbt_splitter->addWidget(ui->parts_body);
    m_splitter->addWidget(ui->editor);

    // изменяем размер разделителей
    QList<int> sizes;
    sizes.append(1);
    sizes.append(250);
    sizes.append(1000);
    m_splitter->setSizes(sizes);
    sizes.clear();

    sizes.append(200);
    sizes.append(1000);
    phb_splitter->setSizes(sizes);
}

// кнопка добавления инструментальной партии
void MainWindow::on_add_inst_clicked()
{
    // запрашиваем у пользователя путь до файла
    inst_path = file->getOpenFileName(this, tr("Выберите wav файл:"), QDir::currentPath());
    // создаём партию
    if (inst_path != "" && inst_path.endsWith(".wav"))
        ui->parts_body->addPart("audio", inst_path);
    else if(inst_path == ""){}
    else {
        // TODO: Сделть проверку на типа файла
        prj->append(inst_path.toStdString());
//        QMessageBox::critical(this, tr("Ошибка"), tr("Данный файл не поддерживается программой!"));
    }
}

// кнопка удаления инструментальной партии
void MainWindow::on_del_inst_clicked()
{
    ui->parts_body->delPart();

}

void MainWindow::connectButtons()
{
    // Подключается синхронизация линий времени
    connect(ui->parts_timeline,  SIGNAL(newTime(int)),
                     ui->parts_body,      SLOT(mainSetTime(int)));
    connect(ui->PR_timeline,  SIGNAL(newTime(int)),
                     ui->parts_body,   SLOT(mainSetTime(int)));

    // Изменение размеров головы партии и сцены, в которой та находится
    connect(ui->parts_head, SIGNAL(resizeHappend(QSize)),
                     ui->parts_body, SLOT(resizeView(QSize)));

    // Изменнение состояния указателя мышки
    connect(ui->SE_tool_pencil, SIGNAL(clicked()), ui->PR_sound, SLOT(setEditMode()));
    connect(ui->SE_tool_hand,  SIGNAL(clicked()), ui->PR_sound, SLOT(setViewMode()));
    connect(ui->SE_tool_pointer, SIGNAL(clicked()), ui->PR_sound, SLOT(setCursorMode()));

    connect(ui->SE_quantize, &QComboBox::currentTextChanged, this,
                     [=](){
                         saveSetStrQuantize(ui->SE_quantize->currentText().remove(0, 2).toInt());
                         if (ui->quantize_edit->currentText() != ui->SE_quantize->currentText()){
                             ui->quantize_edit->setCurrentIndex(ui->SE_quantize->currentIndex());
                         }
                     });
    connect(ui->quantize_edit, &QComboBox::currentTextChanged, this,
                     [=](){
                         saveSetStrQuantize(ui->quantize_edit->currentText().remove(0, 2).toInt());
                         if (ui->quantize_edit->currentText() != ui->SE_quantize->currentText()){
                             ui->SE_quantize->setCurrentIndex(ui->quantize_edit->currentIndex());
                         }
                     });

    // Подключается выключения воспроизведения при нажатии на линии времени
    connect(ui->parts_timeline,  &PartsTimelineGraphicsView::stopPlaying,
            this, &MainWindow::on_button_pause_clicked);
    connect(ui->PR_timeline,  &PRTimelineGraphicsView::stopPlaying,
            this, &MainWindow::on_button_pause_clicked);
    // Подключается переключатель имён партии
    connect(ui->PR_sound, &PianoRollGraphicsView::newPartName,
            this, &MainWindow::newPartName);
    // Подключается изменение наименования партии
    connect(ui->SE_part_edit, &QLineEdit::textChanged,
            ui->PR_sound->PR_scene, &PianoRollGraphicsScene::setPartName);

    // Подключается изменение темпа во всём проекте
    connect(ui->tempo_edit, &QLineEdit::textChanged,
            [=](QString newTempo) { saveSetTempo(newTempo.toInt()); });

    // Подключается сигнал создания нот
    connect(prj->ust(), &Ust::createNote,
            ui->PR_sound, &PianoRollGraphicsView::createNote);
    // Подключаются функция создания партитуры
    connect(prj->ust(), &Ust::createPart, this, &MainWindow::createPart);

    // Устанавливается квантование на 1/8
    ui->quantize_edit->setCurrentIndex(6);
    ui->SE_quantize->setCurrentIndex(6);
}

void MainWindow::setViewStile()
{
    // Убирается обводка, чтобы сливались все три сцены
    ui->PR_sound->setStyleSheet("border: none");
    ui->PR_notes->setStyleSheet("border: none");
    ui->PR_timeline->setStyleSheet("border: none");

    ui->parts_timeline->setStyleSheet("border: none");
    ui->parts_head->setStyleSheet("border: none");
    ui->parts_body->setStyleSheet("border: none");

    // Полоса прокрутки оставляется только у PR_sound и parts_body
    ui->PR_notes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PR_notes->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PR_timeline->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PR_timeline->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->PR_sound->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->PR_sound->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    ui->parts_head->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->parts_head->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->parts_timeline->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->parts_timeline->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->parts_body->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->parts_body->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Устанавливается выравнивание по левому краю
    ui->PR_notes->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->PR_sound->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->PR_timeline->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    ui->parts_body->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->parts_head->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->parts_timeline->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

// Открытие настроек программы
void MainWindow::on_actionPreferences_triggered()
{
    mainSettings->show();
}

// Открытие настроек проекта
void MainWindow::on_actionProject_settings_triggered()
{
    projectSettings->show();
}

void MainWindow::newPRJ_clicked()
{
    createNewProject();
}

void MainWindow::openPRJ_clicked()
{
    openProject();
}

// закрытие программы
void MainWindow::on_actionExite_triggered()
{
    // Если файл не сохранён, предлагаем сохранить
    if(!prj->saved()){
        QMessageBox::question(this,
                              tr("Закрытие программы"),
                              tr("Сохранить проект перед закрытием?"),
                              QMessageBox::Save,
                              QMessageBox::Discard,
                              QMessageBox::Cancel);
    }
    else{
        if(prj->modified()){
            QMessageBox::question(this,
                                  tr("Закрытие программы"),
                                  tr("Сохранить изменения перед закрытием?"),
                                  QMessageBox::Save,
                                  QMessageBox::Discard,
                                  QMessageBox::Cancel);
        }
    }
}

// Создание нового проекта
void MainWindow::createNewProject(){
    prj = new Project();
}

// Открытие проекта
void MainWindow::openProject(){
    // запрашиваем у пользователя путь до файла
    prj_path = QFileDialog::getOpenFileName(this, tr("Выберите wav файл:"),
                                            QDir::currentPath(),
                                            tr("UST     (*.ust *.xpm *.jpg);;"
                                            "VSQX       (*.vsqx);;"
                                            "VOCALOID   (*.vpr);;"
                                            "MIDI       (*.midi)"));
    openProject(prj_path.toStdString());
}

void MainWindow::openProject(const string &path)
{
    prj->open(path);
    ui->tempo_edit->setText(QString::number(prj->ust()->bpm()));
}

// Открытие проекта
void MainWindow::selectedLyrics(QString lyrics, QString syllable){
    ui->SE_lyrics_edit->setText(lyrics);
    ui->SE_symbol_edit->setText(syllable);
}

// Открытие проекта
void MainWindow::newPartName(QString partName){
    ui->SE_part_edit->setText(partName);
}

void MainWindow::on_actionNew_triggered()
{
    createNewProject();
}


void MainWindow::on_actionOpen_triggered()
{
    openProject();
}

void MainWindow::on_actionOpen_append_triggered()
{
    // запрашиваем у пользователя путь до файла
    QString append_path = QFileDialog::getOpenFileName(this, tr("Выберите wav файл:"),
                                            QDir::currentPath(),
                                            tr("UST        (*.ust *.ustx);;"
                                               "VSQX       (*.vsqx);;"
                                               "VOCALOID   (*.vpr);;"
                                               "MIDI       (*.midi)"));
    if (prj){
        prj->start();
        prj->append(append_path.toStdString());
    }
    else{
        prj = new Project(append_path.toStdString());
    }
}

void MainWindow::createPart(std::string partName, std::string voiceDir,
                            std::string cacheDir, std::string flags){
    // Создаётся новая партия
    ui->parts_body->addPart("vocal");
    connect(this, &MainWindow::changeTempo,
            ui->parts_body->body->PRscene(), &PianoRollGraphicsScene::setNewTempo);
    // Подключаются функция масштабирования партитуры
    connect(prj->ust(), &Ust::resizePart, ui->parts_body->body, &PartBody::setLength);
    // Сцена новой партии открывается в редакторе
    ui->PR_sound->setNewScene(ui->parts_body->body->PRscene(), partName.data());
    ui->parts_body->body->PRscene()->setAllItemOldPos(QPoint(0, 0));
    // Задаётся имя партии
    ui->parts_body->head->setName(partName);
    // Передаётся путь до ГБ
    ui->parts_body->body->getVB()->setBasePath(voiceDir);
    // Передаётся путь до директории с кэшем
    ui->parts_body->body->getVB()->setCachDir(cacheDir);
    // Передаются флаги данного проекта
    ui->parts_body->body->getVB()->setFlags(flags);
    // Запускается создание автозаполнителя
    ui->parts_body->body->PRscene()->setCompleter();
    ui->parts_body->body->PRscene()->setPartName(partName.data());
    qDebug() << "UST:" << partName.data();
//    disconnect(prj->ust(), &Ust::resizePart, ui->parts_body->body, &PartBody::setLength);
}
