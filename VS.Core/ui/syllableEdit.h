#ifndef SYLLABLEEDIT_H
#define SYLLABLEEDIT_H

#include <QObject>
#include <QTextCursor>
#include <QCompleter>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollBar>
#include <QAbstractItemView>
#include <QKeyEvent>

class SyllableEdit: public QTextEdit
{
    Q_OBJECT
public:
    SyllableEdit(QWidget *parent = nullptr);
    ~SyllableEdit();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;
    QString syllable() const;
    QString lyrics() const;

    void setType(QString type = "note") {_type = type;}

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

signals:
    void editingFinished();

private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;
    QString completionPrefix;
    QString _type = "note"; // note or line
    QString _syllable = "[a]"; // слог
    QString _lyrics = "a"; // буква(ы)

private:
    QCompleter *c = nullptr;
};

#endif // SYLLABLEEDIT_H
