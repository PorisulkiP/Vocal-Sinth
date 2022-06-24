#ifndef PRGRAPHSCENE_H
#define PRGRAPHSCENE_H

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QFileSystemModel>
#include <QCompleter>
#include <QStringListModel>
#include <QAbstractItemModel>
#include <QMessageBox>
#include <QTextDecoder>
#include <QDebug>

#include "Indicator.h"
#include "Track.h"

class PianoRollGraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    // Сцена для нот, нотного стана и слогов
    explicit PianoRollGraphicsScene(QWidget *parent = nullptr);
    ~PianoRollGraphicsScene();

    // Линия от картеки
    QGraphicsLineItem           *lnk        = 0;
    QLine                       *line       = 0;

    void setCompleter()             { _completer = _VB->completer(); }
    void moveAllItem(qreal x);
    void setAllItemOldPos(QPoint bodyPos);

    void setScenePos(QPointF pos) {setScenePos(pos.toPoint().x(), pos.toPoint().y());}
    void setScenePos(qreal x, qreal y) { setSceneRect(x, y, sceneRect().width(), sceneRect().height()); }
    void setSceneX(qreal x) { setSceneRect(x, sceneRect().y(), sceneRect().width(), sceneRect().height()); }
    void setSceneY(qreal y) { setSceneRect(sceneRect().x(), y, sceneRect().width(), sceneRect().height()); }
    void setSceneW(qreal w) { setSceneRect(sceneRect().x(), sceneRect().y(), w, sceneRect().height());     }
    void setSceneH(qreal h) { setSceneRect(sceneRect().x(), sceneRect().y(), sceneRect().width(), h);      }

    QString partName() const          { return _partName;     }
    QCompleter *completer() const     { return _completer;    }

    Voicebank *getVB() const { return _VB; }
    void setVB(Voicebank *newVB);
    void checkLineCollisions(QGraphicsLineItem *_lnk, bool playState);

    ALuint source() const;
    void setSource(ALuint newSource);

signals:
    void newPartName(QString partName);
    void updateNotes();

public slots:
    void setPartName(QString partName);
    void setNewTempo(double tempo);

private:
    int xOffset = gPRXOffset; // Растояние между цифрами на таймлайне
    int noteHeight = 30; // Высота ноты
    ALuint _source = 0; // Источник звука
    ALuint tmpBuffer = 0;

    Track *track = nullptr; // Нота
    QString _partName; // Имя партии
    QCompleter *_completer = nullptr; // Автозаполнитель
    Voicebank *_VB = nullptr;

    QTextDecoder *decoder = new QTextDecoder(QTextCodec::codecForName("Shift-JIS"));

    // одинарное нажатие ничего не делает
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    // если отвести и отпустить то создастся трек
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    // при перемещении создаём трек до тех пор пока не отпустят мышку
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // создаёт трек фиксированного размера со слогом "а"
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    // Работа с масштабом и положением вида
    virtual void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent) override;

    virtual void keyPressEvent(QKeyEvent *event) override;

    virtual void keyReleaseEvent(QKeyEvent *event) override;

    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};


#endif // PRGRAPHSCENE_H
