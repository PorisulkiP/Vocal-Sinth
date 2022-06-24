#ifndef VBSETTINGSWINDOW_H
#define VBSETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class VBSettingsWindow;
}

class VBSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VBSettingsWindow(QWidget *parent = nullptr);
    ~VBSettingsWindow();

private:
    Ui::VBSettingsWindow *ui;
};

#endif // VBSETTINGSWINDOW_H
