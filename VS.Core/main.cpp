#include "mainwindow.h"
#include "welcomeScreen.h"
#include "test_mainWindow.h"

#include <QtTest/QtTest>
#include <QTextCodec>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSplashScreen>

#define DEBUG 1

void setSplashScreen(QApplication *a, MainWindow *w);
void setWelcomeScreen(MainWindow *w);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Создание главного окна программы
    MainWindow w;

    // Запуск экрана загрузки(SplashScreen)
    setSplashScreen(&a, &w);

    // Создание экрана приветствия
//    setWelcomeScreen(&w);

    // Установка парамтров главного окна
    w.setWindowTitle("Vocal sinth"); // Задаётся имя окна

    // Показ главного окна
    w.show();

    // w.setWindowState(Qt::WindowMaximized); // Растягивание окна на весь экран

    if(a.arguments().size() > 1){
        auto list = a.arguments();
        for (const auto& var : list){
            qDebug() << var;
            w.openProject(var.toStdString());
        }
    }
#ifdef DEBUG
    // Загружается тестовый проект
    w.testFULL_Nemurihime();
//    w.testFULL_CrimsonAndWhiteRedSpiderLilySong();
//    w.testFULL_RIP();
//    w.testFULL_BeautifulFarAway();
    // Загружается тестовое аудио
//    w.testFULL_Shadenfroide();
//    w.openProject("assets/testPrjct/RIP=RELEASE/3 MAIN (CV).ust");
#endif

    // Запуск тестирования
    // QTest::qExec(new Test_MainWindow, argc, argv);

    return a.exec();
}

void setWelcomeScreen(MainWindow *w){
    WelcomeScreen *ws = new WelcomeScreen();

    // Подключаются нажатия на кнопки создания и открытия проекта
    QObject::connect(ws, SIGNAL(newPRJ()), w, SLOT(newPRJ_clicked()));
    QObject::connect(ws, SIGNAL(openPRJ()), w, SLOT(openPRJ_clicked()));

    // Показ экрана приветствия
    ws->show();
}

void setSplashScreen(QApplication *a, MainWindow *w){
    QPixmap pix( "assets/images/SplashScreen.png" );
    QElapsedTimer time;
    QPainter painter;
    if(&pix){
        QSplashScreen splashScreen(pix);
        splashScreen.show();
        a->processEvents();

        int LOAD_TIME_MSEC = 300; // Время запуска программы в мс
        int PROGRESS_WIDTH_PX = 300;
        int PROGRESS_HEIGHT_PX = 30;
        int PROGRESS_X_PX = pix.size().width() - pix.size().width()/1.8;
        int PROGRESS_Y_PX = pix.size().height() - pix.size().height()/2.8;

        time.start();
        while( time.elapsed() < LOAD_TIME_MSEC ) {
            const int progress = static_cast< double >( time.elapsed() ) / LOAD_TIME_MSEC * 100.0;
            splashScreen.showMessage(
                QString::fromUtf8( "Загружено: %1%" ).arg( progress ),
                Qt::AlignBottom | Qt::AlignRight
                );

            painter.begin( &pix );
            painter.fillRect(
                PROGRESS_X_PX,
                PROGRESS_Y_PX,
                progress / 100.0 * PROGRESS_WIDTH_PX,
                PROGRESS_HEIGHT_PX, Qt::darkCyan
                );

            painter.end();

            splashScreen.setPixmap(pix);
            a->processEvents();
        }
        // Закрытие заставки
        splashScreen.finish(w);
    }
}
