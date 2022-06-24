#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QDialog>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QAbstractButton>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>

#include "AL/alc.h"

#include "settingsTabBar.h"
#include "variables.h"

namespace Ui {
class MainSettings;
}

class MainSettings : public QDialog
{
    Q_OBJECT

public:
    explicit MainSettings(QWidget *parent = nullptr);
    ~MainSettings();

    void setDefault();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    void list_audio_devices();

    QWidget *pageGeneral = new QWidget; // Основные настройки
    QWidget *pagePreviews = new QWidget; // Настройка воспроизведения
    QWidget *pageImport = new QWidget; // Настройка импорта файлов
    QWidget *pageOutput = new QWidget; // Настройка выходных данных
    QWidget *pageDiscCache = new QWidget; // Настройка кеширования
    QWidget *pageAppearance = new QWidget; // Настройка внешнего вида интерфейса

    void fillGeneral();
    void fillPreviews();
    void fillImport();
    void fillOutput();
    void fillDiscCache();
    void fillAppearance();

    // General
    QLabel *gName = nullptr;
    QGridLayout *gLayout = nullptr;
    QCheckBox *gIndicZoom = nullptr;
    QCheckBox *gGPUopt = nullptr;
    QCheckBox *gAutosave = nullptr;

    // Previews

    // Import
    QLabel *iName = nullptr;
    QGridLayout *iLayout = nullptr;
    QLabel *iVocalMainPath = nullptr;

    Ui::MainSettings *ui;
};

#endif // MAINSETTINGS_H
