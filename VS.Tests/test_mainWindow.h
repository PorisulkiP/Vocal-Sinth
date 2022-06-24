#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <QObject>

class Test_MainWindow : public QObject
{
    Q_OBJECT

public:
    explicit Test_MainWindow(QObject* parent = 0);
private slots:
    void test_soundNameEdit();
    void test_timelineXoffset();
    void test_loadWAV32bit();
    void test_loadWAV();

};

#endif // TEST_MAINWINDOW_H
