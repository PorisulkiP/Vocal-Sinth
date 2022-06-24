#include "PRNotes.h"

Notes::Notes(PRNotesGraphicsView *_view){
    // устанавливается представление
    view = _view;
    // Создаются ноты и устанавливаются их имена
    notesCreation();
    // Подключается сигнал масштабирования
    connect(view, &PRNotesGraphicsView::resized, this, &Notes::resizeAll);
}

void Notes::notesCreation()
{
    for (int var = 0, num = 0; var < MusicMath::octCount; ++var){
        for(int i = 0; i < MusicMath::noteOneOct; ++i, ++num){
            // Создаётся кнопка
            button = new QPushButton();
            // Устанавливается геометрия ноты
            button->setGeometry(QRect(0, num*height, length, height));
            // На нотах выставляется текст
            button->setText(MusicMath::notesName[num].data());

            // Назначаются стили для тонов и полутонов
            if(MusicMath::notesName[num].find('#') != std::string_view::npos){ // Полутон
                button->setStyleSheet("QPushButton { "
                                      "background-color: black;"                                      
                                      "text-align: right;"
                                      "color: white;"
                                      "border-style: outset;"
                                      "border-width: 2px;"
                                      "border-color: black;"
                                      "}"
                                      "QPushButton:pressed { "
                                      "background-color: rgb(50, 50, 50);"
                                      "border-style: inset;"
                                      "border-color: grey;"
                                      "}");
            }
            else { // Тон
                button->setStyleSheet("QPushButton { "
                                      "background-color: white;"
                                      "text-align: right;"
                                      "padding: 5px;"
                                      "border-style: outset;"
                                      "border-width: 2px;"
                                      "border-color: black;"
                                      "}"
                                      "QPushButton:pressed { "
                                      "color: white;"
                                      "background-color: rgb(50, 50, 50);"
                                      "border-style: inset;"
                                      "border-color: grey;}");
            }
            // Нота добавляется в список нот
            notes.append(button);
            // Ноты добавялются к сцене
            view->scene()->addWidget(button);
        }
    }
}

void Notes::resizeAll(int w) {
    for (int var = 0; var < MusicMath::fullnotesCount; ++var) {
        notes.at(var)->setFixedSize(w, height);
    }
}

void Notes::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsObject::mousePressEvent(event);
}

void Notes::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsObject::mouseReleaseEvent(event);
}

