#include "VBSettingsWindow.h"
#include "../ui_VBSettingsWindow.h"

VBSettingsWindow::VBSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VBSettingsWindow)
{
    ui->setupUi(this);
}

VBSettingsWindow::~VBSettingsWindow()
{
    delete ui;
}
