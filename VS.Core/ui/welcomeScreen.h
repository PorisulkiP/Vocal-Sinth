#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QDialog>
#include <QLabel>

namespace Ui { class WelcomeScreen; }

class WelcomeScreen: public QDialog
{
    Q_OBJECT

public:
    WelcomeScreen(QWidget *parent = nullptr);
    ~WelcomeScreen();

signals:
    void newPRJ();
    void openPRJ();

private slots:
    void on_newPRJ_clicked();

    void on_openPRJ_clicked();

    void on_VS_clicked();

private:
    Ui::WelcomeScreen *ui;
};

#endif // WELCOMESCREEN_H
