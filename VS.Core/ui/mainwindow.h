#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QTimeLine>
#include <QPushButton>
#include <QTimer>
#include <QSplitter>
#include <QFileDialog>
#include <QListWidget>
#include <QUndoCommand>

#include "PRNotes.h"
#include "PRGraphicsScene.h"
#include "partsGraphicsScene.h"
#include "PRNotesGraphicsView.h"
#include "projectSettingsWindow.h"
#include "mainSettings.h"
#include "variables.h"
#include "project.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:    
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void fillDataFromDB();
    void change_mode();
    void change_parts_mode();
    void createNewProject();
    void openProject();
    void openProject(const string &path);
    void connectButtons();
    void setViewStile();

    // TESTS
    void testFULL_BeautifulFarAway();
    void testFULL_Nemurihime();
    void testFULL_CrimsonAndWhiteRedSpiderLilySong();
    void testFULL_Shadenfroide();
    void testFULL_RIP();

    // VB settings
    QListWidgetItem *vocal_name     = new QListWidgetItem();
    QListWidgetItem *vocal_language = new QListWidgetItem();
    QListWidgetItem *vocal_style    = new QListWidgetItem();
    QListWidgetItem *vocal_presets  = new QListWidgetItem();

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

signals:
    void changeTempo(double newTempo);

private slots:
    void on_button_play_clicked();

    void on_button_pause_clicked();

    void on_parts_toolbar_pencil_clicked();

    void on_parts_toolbar_hand_clicked();

    void on_parts_toolbar_pointer_clicked();

    void on_add_inst_clicked();

    void on_del_inst_clicked();

    void on_SE_tool_hand_clicked();

    void on_SE_tool_pointer_clicked();

    void on_SE_tool_pencil_clicked();

    void on_actionProject_settings_triggered();

    void on_actionExite_triggered();

    void newPRJ_clicked();

    void openPRJ_clicked();

    void on_actionPreferences_triggered();

    void selectedLyrics(QString lyrics, QString syllable);

    void newPartName(QString partName);

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionOpen_append_triggered();

    void createPart(std::string partName, std::string voiceDir, std::string cacheDir, std::string flags);

private:

    void splitterConfig();

    // окно настроек программы
    ProjectSettingsWindow *projectSettings  = new ProjectSettingsWindow();
    MainSettings *mainSettings              = new MainSettings();
    Project *prj                            = new Project();

    QString inst_path, prj_path;

    ALCdevice       *device             = nullptr;
    ALCcontext      *context            = nullptr;
    Notes           *notes              = nullptr;
    QFileDialog     *file               = nullptr;
    QMenu           *vocal_pencil_menu  = nullptr;
    QAction         *cut_action         = nullptr;

    Ui::MainWindow *ui;

    // Splitters
    QSplitter *m_splitter   = nullptr; // main_splitter
    QSplitter *p_splitter   = nullptr; // parts_splitter
    QSplitter *phb_splitter = nullptr; // parts_head_body_splitter
    QSplitter *pbt_splitter = nullptr; // parts_body_timeline_splitter
    QSplitter *phe_splitter = nullptr; // parts_header_empty_splitter

};
#endif // MAINWINDOW_H
