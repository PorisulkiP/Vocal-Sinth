#ifndef PARTSGRAPHSCENE_H
#define PARTSGRAPHSCENE_H

#include <QGraphicsScene>

class PartsGraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    // Сцена для инструментала и частей вокала
    PartsGraphicsScene(QWidget *parent = nullptr);
};
#endif // PARTSGRAPHSCENE_H
