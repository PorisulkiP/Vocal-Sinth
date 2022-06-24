#ifndef PARTSGRAPHVIEW_H
#define PARTSGRAPHVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QThread>
#include <QSlider>
#include <QScrollBar>
#include <QDebug>
#include <QResizeEvent>
#include <QtMath>
#include <QTimeLine>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>

#include "audio.h"
#include "project.h"
#include "partHead.h"
#include "PRGraphicsView.h"
#include "PRTimelineGraphicsView.h"
#include "partsHeaderGraphicsView.h"
#include "partsTimelineGraphicsView.h"

class PartsGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    PartsGraphicsView(QWidget *parent = nullptr);
    PartsGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~PartsGraphicsView();

    // setters
    void setEditMode(){
        _edit_mode = true;
        _view_mode = false;
        _cursor_mode = false;
    }

    void setViewMode(){
        _edit_mode = false;
        _view_mode = true;
        _cursor_mode = false;
    }

    void setCursorMode(){
        _edit_mode = false;
        _view_mode = false;
        _cursor_mode = true;
    }
    void setMaking(bool newMode)    { _making = newMode;    }
    void setPressed(bool newMode)   { _pressing = newMode;  }
    void setResizing(bool newMode)  { _resizing = newMode;  }
    void setLength(int newLength)   { _length = newLength;  }
    void setPRsound(PianoRollGraphicsView *_view, PRTimelineGraphicsView *_timeline)
    {
        PR_sound = _view;
        PR_timeline = _timeline;
    }

    // getters
    int partLength()                { return _length;       }
    int partHeight()                { return _height;       }
    bool pressing()                 { return _pressing;     }
    bool resizing() const           { return _resizing;     }
    bool making()                   { return _making;       }
    bool editMode()                 { return _edit_mode;    }
    bool viewMode()                 { return _view_mode;    }
    bool cursorMode()               { return _cursor_mode;  }

    void addPart(QString type, QString path = "");
    void createPart(QString type, QString path);
    void delPart();
    void select(PartBody *body = nullptr, PartHead *head = nullptr);
    void checkLineCollisions(int indPos);

    void cut(); // Разрезаем партию
    void mainPlay();

    QVector<PartHead*>           heads;
    QString                      type       = "vocal";
    QGraphicsProxyWidget        *proxy      = nullptr;
    Audio                       *audio      = nullptr;
    PartHead                    *head       = nullptr;
    PartBody                    *body       = nullptr;
    PartsTimelineGraphicsView   *timeline   = nullptr;
    PartsHeaderGraphicsView     *header     = nullptr;
    // Линия от картеки
    QGraphicsLineItem           *lnk        = nullptr;
    QLine                       *line       = nullptr;

    float newTime = 0; // Новое время для каждого "тела", определяется при изменении положения каретки
    float pxToSec = 50; // Данная переменная зависи от размера сцены и количества секунд в проекте

    void setTimelineHeader(PartsTimelineGraphicsView *_timeline,
                           PartsHeaderGraphicsView *_header);
    void scaleConnect();
    void sliderChange(int eventWidth, int eventHeight);

public slots:
    void resizeView(QSize size);
    void selected(PartBody *body){select(body);}
    void selected(PartHead *head){select(nullptr, head);}
    void mainSetTime(int indPos);
    void onAnimationFinished();

signals:
    void resized();
    void scrolled();

private:
    const int animationPeriodMS = 1000;
    int _width = 1000, _height = 100;
    int _penWidth = 2, _rounded = 2, _length = 1;
    int _select_count = 0, _ypos = 0, yDiff = 0;
    int _slider_width = 0;
    int _xDiff = 0;
    qreal _quantize = 0;
    int _key = 0;

    bool _edit_mode = true;
    bool _view_mode = false;
    bool _cursor_mode = false;

    bool _select, _pressing, _making, _resizing; // нажат, создаётся, масштабируется ли слог
    int magnetPos = 0;
    QPoint _partsPos;

    // переменный для нахождения head при добавлении партии
    QGraphicsItem *item = 0;
    QGraphicsProxyWidget *pProxy = 0;

    // параметры для масштабирования
    QTransform _matrix;
    QSlider *width_scale = new QSlider(Qt::Horizontal, this);
    QSlider *height_scale = new QSlider(Qt::Vertical, this);
    qreal scaleFactor = 0; // Шаг при масштабировании сцены колесом мышки    
    float _zoom_factor = 1.015;
    qreal _width_scale = 1;
    qreal _height_scale = 1;

    // Переменная дл подключения переключения партий в PR_sound
    PianoRollGraphicsView *PR_sound = nullptr;
    PRTimelineGraphicsView *PR_timeline = nullptr;

    // style var
    QColor      color, outlineColor;
    QString     name;
    QBrush      brush;
    QPen        pen;
    QPoint      oldPos, oldMousePos, startPos, newPos;
    QSize       button_size = QSize(_height/4, _height/4); // height/4 = 15
    QSize       slider_size = QSize(_width/2, _height/4);

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void scrollContentsBy(int dx, int dy) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
};
#endif // PARTSGRAPHVIEW_H
