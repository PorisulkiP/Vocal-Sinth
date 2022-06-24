#include <QtTest>
#include <QLineEdit>
#include "test_mainWindow.h"
#include "Windows/mainwindow.h"
#include "../ui_mainwindow.h"

Test_MainWindow::Test_MainWindow(QObject *parent) :
    QObject(parent)
{
    setlocale(LC_ALL,"Russian");
}

// Главная функция при запуске тестов
void Test_MainWindow::test_soundNameEdit()
{
    // Создаётся имя ноты
    QLineEdit SE_sound_name_edit;
    // Имитируется нажатие и ввод
    QTest::keyClicks(&SE_sound_name_edit, "so-");
    // Сравнивается
    QCOMPARE(SE_sound_name_edit.text(), QString("so-"));
    QVERIFY(SE_sound_name_edit.isModified());
}

// Функция тестирования смещения линии времени в редакторе нот
void Test_MainWindow::test_timelineXoffset()
{
//    std::unique_ptr<TimeLine> PR_timeline = std::unique_ptr<TimeLine>(new TimeLine());
//    // Сравнивание значений
//    QCOMPARE(PR_timeline->xOffset, 50);
//    // Проверка на соответствие
//    QVERIFY(PR_timeline->scene == 0);
}

// Функция тестирования загрузки файлов высокой битности
void Test_MainWindow::test_loadWAV32bit()
{
    // Ожидается ошибка
    QEXPECT_FAIL("", "Does not support opening 32 bit WAV files", Continue);
    // Полученная ошибка верифицируется
    QVERIFY(alutCreateBufferFromFile("assets/instruments/Cruella.wav"));
}

// Функция тестирования загрузки инструментальной партии
void Test_MainWindow::test_loadWAV(){
    // Создание главного окна проагрммы
    MainWindow *w = new MainWindow();
    // Загрузка партии
    w->ui->parts_body->addPart("audio", "assets/instruments/Cruella_R.wav");
    // Очищение памяти
    delete w;
}


#include "test_mainWindow.moc"
