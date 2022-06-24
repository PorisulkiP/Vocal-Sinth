#ifndef PROJECTSETTINGSWINDOW_H
#define PROJECTSETTINGSWINDOW_H

#include <QMainWindow>

namespace Ui {
class ProjectSettingsWindow;
}

class ProjectSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectSettingsWindow(QWidget *parent = nullptr);
    ~ProjectSettingsWindow();

private:
    Ui::ProjectSettingsWindow *ui;
};

#endif // PROJECTSETTINGSWINDOW_H
