#include "projectSettingsWindow.h"
#include "../ui_projectSettingsWindow.h"

ProjectSettingsWindow::ProjectSettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProjectSettingsWindow)
{
    ui->setupUi(this);
//    connect(ui->)
}

ProjectSettingsWindow::~ProjectSettingsWindow()
{
    delete ui;
}
