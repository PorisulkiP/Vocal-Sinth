#ifndef TRACK_H
#define TRACK_H

//#define _DEBUG

#include <QGraphicsItem>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QLineEdit>
#include <QList>
#include <QString>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QStringListModel>

#include <cmath>
#include <sstream>
#include <vector>
#include <algorithm>

#include "audio.h"
#include "syllableEdit.h"
#include "trackSettings.h"

using namespace std;
using namespace trackSettings;

class Track: public QGraphicsObject
{
    Q_OBJECT
public:
    Track(QString _name = "a", QGraphicsScene *parent = nullptr);
    ~Track();

    short handleNone  = 0; // правая сторона ноты
    short handleLeft  = 1; // правая сторона ноты
    short handleRight = 2; // левая сторона ноты

    QPoint oldPos, oldMousePos, startPos, endPos;
    SyllableEdit *text = nullptr;
    QLabel *l_syllable = nullptr;
    QBrush brush;
    QPen pen;

    operator QString() const;

    // Перегружаются операторы для сортировки нот в сцене
    friend constexpr bool operator==(const Track &p1, const Track &p2) noexcept
    { return p1.startPos.x() == p2.startPos.x(); }

    friend constexpr bool operator<(const Track &p1, const Track &p2) noexcept
    { return p1.startPos.x() < p2.startPos.x(); }

    friend ostream &operator<<(ostream& s, const Track &t) noexcept
    {
        return s << "\"" << t.lyrics().data() << "\" (Pos: " << t.startPos.x() <<", " << t.startPos.y() <<
               ", Dur: "  << t.length()  << ", Tone:"  << t.getNoteNum() << ")";
    }

    void note_rename(QGraphicsSceneMouseEvent *event);
    void resize(QPoint newPos);
    void moving(QPoint newPos);
    void openAudio();
    void notesFiender(QString syllable);
    void openContextMenu(QPoint screenPos);

    void setLength(int newLength)           { _length = newLength;   }
    void setPressing(bool newMode)          { _pressing = newMode;   }
    void setMaking(bool newMode)            { _making = newMode;     }
    void setResizing(bool newMode)          { _resizing = newMode;   }
    void setNote(string newNote)            { _note = newNote;       }
    void setSyllable(QString _s)            { _syllable = _s;        }
    void setLyrics(QString _l)              { _lyrics = _l;          }
    void setColor(QColor color)             { _color = color;        }
    // Установка стандартнго цвета ноте
    void setMainColor()                     { _color = mainColor;    }
    // Установка нового стандартного цвета ноты
    void setMainColor(QColor color)         { mainColor = color;    }
    // Установка выделения ноты
    void setSelected(bool selected);
    // Установка типа масштабирования
    void setTypeOfResize(short type)        { _typeOfResize = type;  }
    void setEndPos(QPoint pos)              { endPos = pos;          }
    // Установка дополнителя
    void setCompleter(QCompleter *completer){
        _completer = completer;
        // Автозаполнитель устанавливается в поле ввода
        text->setCompleter(_completer);
    }
    // Установка вибрато
    void setVibrato(Vibrato *newVibrato);
    void setOldPos(QPoint pos)         { oldPos = pos;}
    void setOldMousePos(QPoint pos)    { oldMousePos = pos;}

    enum                            { Type = UserType + 1   };
    int type() const                { return Type;          }
    QString note() const            { return QString(_note.data());}
    QString lyrics() const          { return _lyrics;       }
    QString syllable() const        { return _syllable;     }
    QColor color() const            { return _color;        }
    int length() const              { return _length;       }
    int height() const              { return _height;       }
    short typeOfResize() const      { return _typeOfResize; }
    bool pressed() const            { return _pressing;     }
    bool maked() const              { return _making;       }
    bool resized() const            { return _resizing;     }
    bool isSelected() const         { return _selected;     }
    Pitch *pitch()                  { return _pitch;        }
    Vibrato *vibrato()              { return _vibrato;      }
    Audio *audio()                  { return _audio;        }

    int getDelta() const {return delta;}
    void setDelta(int newDelta) {delta = newDelta;}

    int getDuration() const {return duration;}
    void setDuration(int newDuration) {duration = newDuration;}

    int getNoteNum() const {return noteNum;}
    void setNoteNum(int newNoteNum);

    int getIntensity() const {return intensity;}
    void setIntensity(int newIntensity) {intensity = newIntensity;}

    int getModulation() const {return modulation;}
    void setModulation(int newModulation) {modulation = newModulation;}

    short getPBType() const {return PBType;}
    void setPBType(short newPBType) {PBType = newPBType;}

    int getOverlap() const {return overlap;}
    void setOverlap(int newOverlap) {overlap = newOverlap;}

    int getPreutter() const {return preutter;}
    void setPreutter(int newPreutter) {preutter = newPreutter;}

    int getNoteTempo() const {return noteTempo;}
    void setNoteTempo(int newNoteTempo);

    vector<float> *getEnvelope() {return &envelope;}
    void setEnvelope(vector<float> *newEnvelope) {envelope = *newEnvelope;}

    vector<int> *getPBW() {return &PBW;}
    void setPBW(vector<int> *newPBW) {PBW = *newPBW;}

    vector<int> *getPBS() {return &PBS;}
    void setPBS(vector<int> *newPBS) {PBS = *newPBS;}

    vector<int> *getPBY() {return &PBY;}
    void setPBY(vector<int> *newPBY) {PBY = *newPBY;}

    vector<int> *getPBM() {return &PBM;}
    void setPBM(vector<int> *newPBM) {PBM = *newPBM;}

    Voicebank *getVB() const { return VB; }
    void setVB(Voicebank *newVB);

    QPoint getOldPos() const;

signals:
    void newLyrics(QString lyrics);
    void select(Track *t);

public slots:
    void lyricsEditHide();

private:
    bool _selected, _pressing, _making, _resizing; // нажат, создаётся, масштабируется ли слог
    int _length, _height;
    short _typeOfResize = 0; // сторона, которая масштабируется

    Voicebank *VB = nullptr;

    int delta = 0;
    int duration = 0;
    int noteNum = 60; // Высота ноты
    int intensity = 100;
    int modulation = 0;
    short PBType = 0;
    int overlap = 0;
    int preutter = 0;
    int noteTempo = 100;
    vector<float> envelope;
    vector<int> PBW;
    vector<int> PBS;
    vector<int> PBY;
    vector<int> PBM;

    // Высота звука
    Pitch *_pitch = new Pitch();
    // Вибрация ноты
    Vibrato *_vibrato = new Vibrato();
    // Звук
    Audio *_audio = new Audio();

    QString _syllable = "[a]"; // слог
    QString _lyrics = "a"; // буква(ы)
    string _note, full_filename; // имя = слог, нота = знак с нотного стана

    QCompleter *_completer = nullptr;
    QGraphicsScene *_parent = nullptr;
    QColor _color, mainColor = QColor(Qt::cyan);

    int _yDiff, _xDiff, penWidth, rounded;

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // TRACK_H






