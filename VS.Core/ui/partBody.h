#ifndef PARTBODY_H
#define PARTBODY_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QPainter>
#include <QVector>
#include <QWidget>
#include <QObject>
#include <QPoint>
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QFileDialog>

#include <future>
#include <thread>
#include <chrono>

#include "audio.h"
#include "voicebank.h"

// Сцены для каждой части партии
#include "PRGraphicsScene.h"

class PartBody: public QGraphicsObject
{
    Q_OBJECT
public:
    PartBody(QWidget *_parent = nullptr, QString part_type = "vocal");
    PartBody(const PartBody &ref);
    ~PartBody();

    // У "головы" вокальной партии может быть несколько "тел"
    QVector<PartBody*> bodies;
    QWidget *head;

    short handleNone  = 0; // правая сторона ноты
    short handleLeft  = 1; // правая сторона ноты
    short handleRight = 2; // левая сторона ноты

    void drawHistogram();
    void drawNotes();
    void move(QPoint newPos);
    void move(qreal x)                              { move(QPoint(x, pos().y()));}
    void move(qreal x, qreal y)                     { move(QPoint(x, y));        }
    void resize(int width, int height);
    void resize(QPoint newPos);
    void openContextMenu(QPointF screenPos)
    { openContextMenu(screenPos.toPoint()); }
    void openContextMenu(QPoint screenPos);

    template<class PRScene>
    void setScene(PRScene *newScene)                { _scene = newScene;        }
    void setResizing(bool newMode)                  { _resizing = newMode;      }
    void setTypeOfResize(short type)                { _typeOfResize = type;     }
    void setColor(QColor color)                     {  _color = color;          }
    void setHead(QWidget *_head)                    {  head = _head;            }
    void setPartType(QString part_type)             {  _part_type = part_type;  }
    void setPressed(bool newMod)                    { _pressing = newMod;       }
    void setType(QString _type)                     { _part_type = _type;       }
    void setID(int id)                              { _id = id;                 }
    void setHeight(int height)                      { _height = height;         }
    void setOldPos(QPoint pos)                      { _oldPos = pos;            }
    void setOldMousePos(QPoint pos)                 { _oldMousePos = pos;       }
    void setSamples(QList<float> *_points)          { setLSamples(_points); }
    void setRSamples(QList<float> *RSamples)        { _RSamples = *RSamples; }
    void setLSamples(QList<float> *LSamples){ _LSamples = *LSamples; drawHistogram();    }
    void setSelect(bool newMod);
    void setAudio(Audio *a);

    // Функции установки цветов
    void setMainVColor()        { _color = baseVColor; }
    void setMainWColor()        { _color = baseWColor; }
    void setSelectedVColor()    { _color = baseVColor; }
    void setSelectedWColor()    { _color = baseWColor; }

    enum                                { Type = UserType + 1   };
    int type() const                    { return Type;          }
    int id() const                      { return _id;           }
    int length() const                  { return _length;       }
    int height() const                  { return _height;       }
    short typeOfResize() const          { return _typeOfResize; }
    bool selected() const               { return _select;       }
    bool pressed() const                { return _pressing;     }
    bool maked() const                  { return _making;       }
    bool resized() const                { return _resizing;     }
    QPoint oldPos() const               { return _oldPos;       }
    QString part() const                { return _part_type;    }
    QString comment() const             { return _comment;      }
    QColor color() const                { return _color ;       }
    Audio *audio() const                { return _audio;        }
    PianoRollGraphicsScene *PRscene()   { return _scene;        }

    Voicebank *getVB() const {return VB;}
    void setVB(Voicebank *newVB) {VB = newVB;}
    virtual QRectF boundingRect() const override{return QRectF(0, 0, _length, _height);}

signals:
    void select(PartBody *body);
    void changeScene(PianoRollGraphicsScene *_scene, QString partName);

public slots:
    void setLength(qreal width);
    void updateHistogram(){ drawHistogram(); }
    void updateNotes(){ drawNotes(); }

private:
    std::future<void> futureDrawHistogram;
    std::future<void> notesDrawHistogram;
    Voicebank *VB = new Voicebank();
    Audio *_audio = nullptr;
    std::mutex _mutex;

    ALuint _source = 0; // Источник звука

    PianoRollGraphicsScene *_scene = new PianoRollGraphicsScene();
    QColor _color;
    QPixmap *_pixmap = new QPixmap();
    QPainter *_painter = nullptr;
    QString _part_type = "vocal";
    QString _comment;
    int _length = 0;
    int  _id = 0;
    int _height = 0;
    short _typeOfResize = 0; // сторона, которая масштабируется

    QList<float> points;
    QList<float> _LSamples;
    QList<float> _RSamples;

    QColor baseVColor = QColor(125, 150, 73, 50);
    QColor selectedVColor = QColor(125, 150, 73, 80);

    QColor baseWColor = QColor(130, 105, 182, 50);
    QColor selectedWColor = QColor(130, 105, 182, 80);

    QPoint _partsPos, _newPos, _oldPos, _oldMousePos;
    bool _select, _pressing, _making, _resizing; // нажат, создаётся, масштабируется ли слог

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // PARTBODY_H
