#include "welcomeScreen.h"
#include "ui_welcomeScreen.h"

WelcomeScreen::WelcomeScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::FramelessWindowHint);
}

WelcomeScreen::~WelcomeScreen(){}


void WelcomeScreen::on_newPRJ_clicked()
{
    emit newPRJ();
}


void WelcomeScreen::on_openPRJ_clicked()
{
    emit openPRJ();
}


void WelcomeScreen::on_VS_clicked()
{
    emit newPRJ();
}

