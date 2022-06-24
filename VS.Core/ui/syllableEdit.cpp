#include "syllableEdit.h"

#include <QDebug>

SyllableEdit::SyllableEdit(QWidget *parent): QTextEdit(parent){}

SyllableEdit::~SyllableEdit(){}

void SyllableEdit::setCompleter(QCompleter *completer)
{
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setFilterMode(Qt::MatchStartsWith);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, QOverload<const QString &>::of(&QCompleter::activated),
                     this, &SyllableEdit::insertCompletion);
}

void SyllableEdit::insertCompletion(const QString &completion)
{
    if (c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    _lyrics = completion;
//    _syllable = completion;
    clear();
    tc.insertText(_lyrics);
//    if (_type == "note"){
//        tc.insertText(_lyrics + " [" + _syllable + "]");
//    }
//    else{
//        tc.insertText("[" + _syllable + "]");
//    }

    setTextCursor(tc);
}

QCompleter *SyllableEdit::completer() const
{
    return c;
}

QString SyllableEdit::syllable() const
{
    return _syllable;
}

QString SyllableEdit::lyrics() const
{
    return _lyrics;
}

QString SyllableEdit::textUnderCursor() const
{
    QTextCursor tc = QTextCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void SyllableEdit::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void SyllableEdit::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);
    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;

    if (_type == "note"){
        // Из поля ноты достаётся слог
        completionPrefix = textUnderCursor();
        completionPrefix = toPlainText();
//        _syllable = "";
        _lyrics = completionPrefix.remove("\n");


//        for (int i = _lyrics.indexOf("["); i < _lyrics.count(); i++) {
//            _syllable.append(_lyrics.at(i));
//        }

//        if (_lyrics.length() > 0) {
//            _lyrics.remove(_syllable);
//        }
    }
    else {
        // Если из поля вне ноты достаётся слог
        completionPrefix = textUnderCursor();
        completionPrefix = toPlainText();
        _lyrics = completionPrefix;
//        _syllable = completionPrefix.remove("\n").remove("[").remove("]");

//        qDebug() << "_syllable 1 " << _syllable;
    }

    // Проверка на то, что там не пусто
    if (!isShortcut && (hasModifier || e->text().isEmpty()))
    {
        c->popup()->hide();
        return;
    }

    if (_lyrics != c->completionPrefix()) {
        c->setCompletionPrefix(_lyrics);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    // Устанавливается размер области в которой показываются предложения по вводу
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // Вызывается меню с подсказками

    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return){
        if (c->currentCompletion() != ""){
            emit c->activated(c->currentCompletion());
        }
        clearFocus();
        emit editingFinished();
    }
}

void SyllableEdit::focusOutEvent(QFocusEvent *e){
    QTextEdit::focusOutEvent(e);
    if (c) { c->popup()->hide(); }
    hide();
}
