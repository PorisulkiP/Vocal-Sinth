#ifndef PARTHEAD_H
#define PARTHEAD_H

#include <QGraphicsItem>
#include <QPushButton>
#include <QSlider>
#include <QVector>
#include <QWidget>
#include <QObject>
#include <QPoint>
#include <QComboBox>
#include <QDebug>

#include "partBody.h"
#include "project.h"

class PartHead: public QWidget
{
    Q_OBJECT
public:
    explicit PartHead(QWidget *_parent = nullptr, QString part_type = "vocal");
    ~PartHead(){}

    // У "головы" вокальной партии может быть несколько "тел"
    QVector<PartBody*> bodies;    

    void headPlacement(int width, int height);

    // setters
    void setColor(QColor color);
    void setName(std::string newName)   { head_name->setText(newName.data());   }
    void setPressed(bool newMod)        { _pressing = newMod;                   }
    void setType(QString _type)         { _part_type = _type;                   }
    void setWidth(qreal width)          { resize(width, size().height());       }
    void setHeight(int height)          { resize(size().width(), height);       }
    void setID(int id)                  { _id = id;                             }
    void setHead(PartHead *head);
    void setSelectAll(bool newMod);
    void setSelect(bool newMod);    
    void setAudio(Audio *a){
        _audio = a;
        QObject::connect(head_mute_btn, &QPushButton::clicked,
                         _audio, _audio->mute);
    }

    // Функции установки цветов
    void setMainVColor()        { setColor(baseVColor);      }
    void setMainWColor()        { setColor(baseWColor);      }
    void setSelectedVColor()    { setColor(selectedVColor);  }
    void setSelectedWColor()    { setColor(selectedWColor);  }

    // getters
    int id()                { return _id ;          }
    bool isSomeSelected();
    bool selected()         { return _select;       }
    bool pressed()          { return _pressing;     }
    QString part_type()     { return _part_type;    }
    std::string name()      { return head_name->text().toStdString();    }
    QColor color()          { return _color;        }
    Audio *audio()          { return _audio;        }

    QPushButton *head_mute_btn;
    QSlider* vol, *direction;

signals:
    void select(PartHead *part);

private:
    Audio *_audio = 0;
    // текущий цвет элемента
    QColor _color;
    // Цвета если голова относится к голосовой партии
    QColor baseVColor = QColor(125, 150, 73, 50);
    QColor selectedVColor = QColor(125, 150, 73, 80);
    // Цвета если голова относится к инструментальной партии
    QColor baseWColor = QColor(130, 105, 182, 50);
    QColor selectedWColor = QColor(130, 105, 182, 80);

    // Кнопки настроек для головы партии
    QPushButton* head_up_btn, *head_solo_btn, *head_down_btn;
    QLineEdit* head_name, *vol_value, *direction_value;
    QSize button_size = QSize(height()/6, height()/6);
    QSize slider_size = QSize(width()/2, height()/6);
    QComboBox *VBitemsComBox = nullptr;

    // Длинна, высота и номер головы партии
    int _id = 0, slider_width = 0;
    short _typeOfResize = 0; // сторона, которая масштабируется
    QString _part_type = "vocal";

    bool _select, _pressing; // нажат или выбран ли слог

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void moveEvent(QMouseEvent *event);
};

#endif // PARTHEAD_H
