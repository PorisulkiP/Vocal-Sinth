#include "mainSettings.h"
#include "../ui_mainSettings.h"

MainSettings::MainSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainSettings)
{
    ui->setupUi(this);

    fillGeneral();
//    fillPreviews();
    fillImport();
//    fillOutput();
//    fillDiscCache();
//    fillAppearance();

    SettingsTabWidget *tab = new SettingsTabWidget();
    tab->setTabPosition(SettingsTabWidget::West);
    tab->addTab(pageGeneral, QObject::tr("General"));
    tab->addTab(pagePreviews, QObject::tr("Previews"));
    tab->addTab(pageImport, QObject::tr("Import"));
    tab->addTab(pageOutput, QObject::tr("Output"));
    tab->addTab(pageDiscCache, QObject::tr("Disc cache"));
    tab->addTab(pageAppearance, QObject::tr("Appearance"));

    // Получение всех аудио выходов
//    list_audio_devices();

    QGridLayout *gridLayout = new QGridLayout(this);

    gridLayout->addWidget(tab);
    gridLayout->addWidget(ui->buttonBox);
}

MainSettings::~MainSettings()
{
    delete ui;
}

void MainSettings::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)){ // Кнопка по-умолчанию
        qDebug() << "Нажали сброс";
        setDefault();
    }
    else if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Save)){ // кнопка сохранения
        qDebug() << "Нажали сохранить";
    }
    else{ // Кнопка отмены
        qDebug() << "Нажали отмену";
    }
}

void MainSettings::setDefault(){

}

void MainSettings::fillGeneral(){
    // Создаём имя данного подраздела
    gName = new QLabel("Genaral", pageGeneral);
    gName->setAlignment(Qt::AlignHCenter);

    // Создаётся разместитель
    gLayout = new QGridLayout(pageGeneral);
    gLayout->setContentsMargins(width()/50, height()/50, 0, 0);
    gLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Создаются элементы настроек
    gIndicZoom = new QCheckBox(pageGeneral);
    gIndicZoom->setText(QObject::tr("Масштабировать редакторы относительно картеки времени"));
    gIndicZoom->setChecked(true);

    gGPUopt = new QCheckBox(pageGeneral);
    gGPUopt->setText(QObject::tr("Использовать ускорение GPU"));
    gGPUopt->setChecked(false);

    gAutosave = new QCheckBox(pageGeneral);
    gAutosave->setText(QObject::tr("Автосохранение"));
    gAutosave->setChecked(true);

    // Объекты добавляются в расстановщик
    gLayout->addWidget(gName);
    gLayout->addWidget(gIndicZoom);
    gLayout->addWidget(gGPUopt);
    gLayout->addWidget(gAutosave);
}

void MainSettings::fillImport(){
    // Создаём имя данного подраздела
    iName = new QLabel("Import", pageImport);
    iName->setAlignment(Qt::AlignHCenter);

    // Создаётся разместитель
    iLayout = new QGridLayout(pageImport);
    iLayout->setContentsMargins(width()/50, height()/50, 0, 0);
    iLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Создаются элементы настроек
    iVocalMainPath = new QLabel(pageImport);
    iVocalMainPath->setText(QObject::tr("C:\\Program Files (x86)\\UTAU\\"));

    auto vocalPathName = new QLabel("Path to voice folder: ", pageImport);
    vocalPathName->setAlignment(Qt::AlignLeft);

    // Кнопка сброса пути до папки с ГБ-ми
    auto reset = new QPushButton("Reset", pageImport);
    // Кнопка установки пути до папки с ГБ-ми
    auto select = new QPushButton("Select", pageImport);

    connect(reset, &QPushButton::clicked, [&](){
        iVocalMainPath->setText(QObject::tr("C:\\Program Files (x86)\\UTAU\\"));
    });

    connect(select, &QPushButton::clicked, [&](){
        auto prj_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                             "C:\\Program Files (x86)\\UTAU\\",
                                             QFileDialog::ShowDirsOnly
                                           | QFileDialog::DontResolveSymlinks);
        iVocalMainPath->setText(prj_path);
        saveSetMainVBFolder(prj_path.toStdString().data());

    });

    auto tmp = new QGridLayout();
    tmp->addWidget(vocalPathName, 0, 0);
    tmp->addWidget(iVocalMainPath, 0, 1);
    tmp->addWidget(reset, 1, 0);
    tmp->addWidget(select, 1, 1);

    // Объекты добавляются в расстановщик
    iLayout->addWidget(iName, 0, 0);
    iLayout->addLayout(tmp, 1, 0);

}

void MainSettings::list_audio_devices()
{
    const ALCchar *devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    const ALCchar *device = devices, *next = devices + 1;
    size_t len = 0;

    qDebug() << "Devices list:";
    qDebug() << "----------";
    while (device && *device != '\0' && next && *next != '\0') {
        qDebug() <<  device;
        len = strlen(device);
        device += (len + 1);
        next += (len + 2);
    }
    qDebug() <<  "----------";
}

