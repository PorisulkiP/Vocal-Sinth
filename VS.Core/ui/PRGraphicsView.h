#ifndef NOTEGRAPHVIEW_H
#define NOTEGRAPHVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSlider>
#include <QtMath>
#include <QDebug>

#include "PRTimelineGraphicsView.h"
#include "partsTimelineGraphicsView.h"
#include "PRGraphicsScene.h"
#include "PRNotes.h"
#include "project.h"

class PianoRollGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    PianoRollGraphicsView(QWidget *parent = nullptr);
    PianoRollGraphicsView(PianoRollGraphicsScene *PR_scene, QWidget *parent = nullptr);
    ~PianoRollGraphicsView();

    PRTimelineGraphicsView *timeline = 0;
    PianoRollGraphicsScene *PR_scene = 0;
    PRNotesGraphicsView *notes = 0;

    void sliderChange(int eventWidth, int eventHeight);
    void scaleConnect();
    void checkLineCollisions(int indPos);

    void setSceneX(qreal x) { setSceneRect(x, sceneRect().y(), sceneRect().width(), sceneRect().height()); }
    void setSceneY(qreal y) { setSceneRect(sceneRect().x(), y, sceneRect().width(), sceneRect().height()); }
    void setSceneW(qreal w) { setSceneRect(sceneRect().x(), sceneRect().y(), w, sceneRect().height());     }
    void setSceneH(qreal h) { setSceneRect(sceneRect().x(), sceneRect().y(), sceneRect().width(), h);      }

    // Функция восстановление режима после нажатия спец клавиш
    void setLastMode(bool edit_mode, bool view_mode, bool cursor_mode){
        temp_edit_mode = edit_mode;
        temp_view_mode = view_mode;
        temp_cursor_mode = cursor_mode;
    }

    void setMaking(bool newMode)        { _making = newMode;    }
    void setPressing(bool newMode)      { _pressing = newMode;  }
    void setLyricsText(SyllableEdit *LE);
    void setSyllableText(SyllableEdit *syllableText);

    void setAll(PRTimelineGraphicsView *_timeline, PRNotesGraphicsView *_notes);
    void setPRScene(PianoRollGraphicsScene *_scene);

    QString partName()  { return PR_scene->partName();  }
    bool editMode()     { return _edit_mode;            }
    bool viewMode()     { return _view_mode;            }
    bool cursorMode()   { return _cursor_mode;          }
    bool pressed()      { return _pressing;             }
    bool maked()        { return _making;               }
    void lastMode(){
        if (temp_edit_mode)
            setEditMode();
        else if (temp_view_mode)
            setViewMode();
        else if (temp_cursor_mode)
            setCursorMode();
    }

signals:
    void selectedLyrics(QString lyrics, QString syllable);
    void newPartName(QString partName);

public slots:
    // Слот для смены сцены
    void setNewScene(PianoRollGraphicsScene *_scene = nullptr, QString _partName = "");
    // Слот для выделения
    void select(Track* track = nullptr);
    // Слот для создания ноты
    void createNote(int length = 100, int delta = 0, int duration = 100, std::string lyric = "a", int noteNum = 60,
                    int intensity = 100, int modulation = 0, short PBType = 0, int overlap = 0, int preutter = 0,
                    Vibrato *vibrato = nullptr, std::vector<float> *envelope = nullptr, std::vector<int> *PBS = nullptr,
                    std::vector<int> *PBW = nullptr, std::vector<int> *PBY = nullptr, std::vector<int> *PBM = nullptr,
                    int noteTempo = 100, int XPos = 0);

    void setEditMode(){
        _edit_mode = true;
        _view_mode = false;
        _cursor_mode = false;

        setDragMode(QGraphicsView::NoDrag);
        setCursor(editCursor);
    }

    void setViewMode(){
        _edit_mode = false;
        _view_mode = true;
        _cursor_mode = false;

        setCursor(Qt::ArrowCursor);
        setDragMode(QGraphicsView::ScrollHandDrag);
    }

    void setCursorMode(){
        _edit_mode = false;
        _view_mode = false;
        _cursor_mode = true;

        setCursor(Qt::ArrowCursor);
        setDragMode(QGraphicsView::RubberBandDrag);
    }

private:
    // переменные отвечающие за режим работы редактора
    // temp нужны для востановления режима после кратковременного изменения
    bool temp_edit_mode, _edit_mode = true;
    bool temp_view_mode, _view_mode = false;
    bool temp_cursor_mode, _cursor_mode = false;

    bool _making = false; // Создаётся ли сейчас ли слог
    bool _pressing = false; // Нажат ли сейчас слог
    bool _resizing = false; // Мосштабируется ли слог
    int _key = 0; // хранит нажатую кнопку из keyPressEvent
    int _selectedCount = 0; // Кол-во выделенных партий
    int note_num = 0; // номер ноты, выдаётся слоговой ноте при создании
    int _xDiff = 0; // Разница позиции ноты по оси Х
    int xOffset = gPRXOffset; // Растояние между цифрами на таймлайне
    int noteHeight = 30; // Высота нот

    // Позиции для перемещения нот с уловием их высоты и квантования
    qreal magnetPosX = 0, magnetPosY = 0;

    // Курсор-карандаш для режима редактирования
    QCursor editCursor = QCursor(QPixmap("assets/icons/cursor-pencel.png").scaled(20, 20));

    SyllableEdit *_syllableText = 0;

    // Линия от картеки
    QGraphicsLineItem           *lnk        = 0;
    QLine                       *line       = 0;

    Track* track = nullptr;
    QGraphicsItem* item = 0;
    QGraphicsProxyWidget* proxy = 0;

    QPoint newPos, oldPos, oldMousePos;
    QPoint startPoint = QPoint(0, -30); // Стартовая позиция редактора

    // параметры для масштабирования
    QTransform _matrix; // Матрица для масштабирования редактора
    QSlider *width_scale = new QSlider(Qt::Horizontal, this);
    QSlider *height_scale = new QSlider(Qt::Vertical, this);
    qreal scaleFactor = 0; // Шаг при масштабировании сцены колесом мышки
    float _zoo_factor = 1.015; // Шаг при масштабировании сцены ползунками
    qreal _width_scale = 1;
    qreal _height_scale = 1;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    // одинарное нажатие ничего не делает
    void mousePressEvent(QMouseEvent *event) override;

    // если отвести и отпустить то создастся трек
    void mouseReleaseEvent(QMouseEvent *event) override;

    // при перемещении создаём трек до тех пор пока не отпустят мышку
    void mouseMoveEvent(QMouseEvent *event) override;

    // создаёт трек фиксированного размера со слогом "а"
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    // создаёт трек фиксированного размера со слогом "а"
    void resizeEvent(QResizeEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    // Двигает вместе с данным представлением нотный стан и временную шкалу
    void scrollContentsBy(int dx, int dy) override;
};
#endif //noteGraphicsView.h
