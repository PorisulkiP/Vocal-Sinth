#include "partHead.h"

PartHead::PartHead(QWidget *_parent, QString part_type){
    _part_type = part_type;
    if (_part_type == "vocal"){
        head_name = new QLineEdit("1 VOCALIST", this);
        setMainVColor();
        VBitemsComBox = new QComboBox(this);
        for (const auto& var : Project::VBitems) {
            VBitemsComBox->addItem(var.data());
        }
    }
    else{
        head_name = new QLineEdit("1 INSTRUMENTAL", this);
        setMainWColor();
    }

    // Для головы создаём кнопки
    head_up_btn = new QPushButton("˄", this);
    head_down_btn = new QPushButton("˅", this);
    head_solo_btn = new QPushButton("S", this);
    head_solo_btn->setCheckable(true);
    head_mute_btn = new QPushButton("M", this);
    head_mute_btn->setCheckable(true);
    vol = new QSlider(Qt::Horizontal, this);
    vol_value = new QLineEdit("0", this);
    direction = new QSlider(Qt::Horizontal, this);
    direction_value = new QLineEdit("0", this);

    setFocusPolicy(Qt::StrongFocus);

    // Подключаются ползунки с громкостью и положением
    QObject::connect(vol, &QSlider::valueChanged, this, [=](){
        if (vol->value() != vol_value->text().toInt()){
            vol_value->setText(QString::number(vol->value()));
        }
    });

    // По нажатию на клавишу ENTER если текст изменён,
    // то изменяется положение ползунка
    QObject::connect(vol_value, &QLineEdit::editingFinished, this, [=](){
        if (vol->value() != vol_value->text().toInt()){
            vol->setValue(vol_value->text().toInt());
        }
        clearFocus();
    });

    QObject::connect(direction, &QSlider::valueChanged, this, [=](){
        if (direction->value() != direction_value->text().toInt()){
            direction_value->setText(QString::number((int)direction->value()));
        }
    });

    QObject::connect(direction_value, &QLineEdit::editingFinished, this, [=](){
        if (direction->value() != direction_value->text().toInt()){
            direction->setValue(direction_value->text().toInt());
        }
        clearFocus();
    });
}

// ВЕСЬ ЭТОТ КОШМАР НАДО ПЕРЕДЕЛАТЬ ПОД QGridLayout
void PartHead::headPlacement(int width, int height){
    resize(width, height);

    // размеры кнопок и слайдеров
    button_size = QSize(width/13, height/5);
    slider_size = QSize(width/1.75, height/5);

    // Отступы
    auto width_indent = width/20;
    auto height_indent = height/20;

    // Позиции кнопок и слайдеров
    QPoint button_pos = QPoint(width / 40, height/2);
    QPoint slider_pos = QPoint(width/8, height/2);

    QSize value_size = QSize(height/3.5, height/5);

    head_name->move(width/8, 0); // width/8 = 25
    head_name->resize(width/1.25 - width/10, height/5);

    if (_part_type == "vocal"){
        VBitemsComBox->move(width/8, height/4);
        VBitemsComBox->resize(width/1.25 - width/10, height/5);
    }

    head_up_btn->move(button_pos);
    head_up_btn->resize(button_size);

    head_down_btn->move(button_pos.x(), button_pos.y() + button_size.height() + height_indent);
    head_down_btn->resize(button_size);

    vol->move(slider_pos);
    vol->resize(slider_size);
    vol->setRange(0, 1000);

    direction->move(slider_pos.x(), slider_pos.y() + slider_size.height() + height_indent);
    direction->resize(slider_size);
    direction->setRange(-100, 100);

    //
    vol_value->move(slider_pos.x() + slider_size.width() + button_size.width() - width_indent, height/2);
    vol_value->resize(value_size);
    vol_value->setAlignment(Qt::AlignCenter);

    direction_value->move(slider_pos.x() + slider_size.width() + button_size.width() - width_indent, height/1.3);
    direction_value->resize(value_size);
    direction_value->setAlignment(Qt::AlignCenter);

    //
    head_solo_btn->move(slider_pos.x() + slider_size.width() + width_indent + button_size.width() * 2, height/2);
    head_solo_btn->resize(button_size);

    head_mute_btn->move(slider_pos.x() + slider_size.width() + width_indent + button_size.width() * 2, height/1.3);
    head_mute_btn->resize(button_size);


    vol_value->setStyleSheet("padding-left: 1px");
    direction_value->setStyleSheet("padding-left: 1px");
}

bool PartHead::isSomeSelected(){
    for (int i = 0; i < bodies.count(); i++) {
        if(bodies.at(i)->selected())
            return true;
    }
    return false;
}

void PartHead::setSelectAll(bool newMod) {
    setSelect(newMod);
    for (int i = 0; i < bodies.count(); i++) {
        bodies.at(i)->setSelect(newMod);
    }
}

void PartHead::setColor(QColor color) {
    _color = color;
    setStyleSheet(QString("background-color: "
                          "rgba(%1, %2, %3, %4)").arg(_color.red()).arg(_color.green())
                                                 .arg(_color.blue()).arg(_color.alpha()));
}

void PartHead::setSelect(bool newMod) {
    _select = newMod;

    // Устанавливаются цвета в зависимости от состояния выделения
    if (_part_type == "vocal"){
        if (!_select)   { setMainVColor();      }
        else            { setSelectedVColor();  }
    }
    else{ // Wave
        if (!_select)   { setMainWColor();      }
        else            { setSelectedWColor();  }
    }
}

void PartHead::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    emit select(this);
}

void PartHead::mouseMoveEvent(QMouseEvent *event){
    QWidget::mouseMoveEvent(event);
}

void PartHead::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void PartHead::moveEvent(QMouseEvent *event)
{
//    QWidget::moveEvent(event);
    foreach (PartBody* var, bodies) {
        var->setPos(event->pos());
    }

}
