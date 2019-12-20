#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QGraphicsLineItem>
#include <QLineF>
#include <QStringList>
#include <QTableWidget>
#include <QMutex>
#include <QMenu>
#include <memory>
#include <QGraphicsScene>

#include "deletemenu.h"


qreal toCoordinates(qreal value, bool axis);
qreal fromCoordinates(qreal value, bool axis);

class MoveItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit MoveItem(QObject *parent = 0);
    ~MoveItem();

    void setInLine(QGraphicsLineItem *in);
    void setOutLine(QGraphicsLineItem *out);
    void setPoints(QVector<std::shared_ptr<MoveItem> > * vector);
    void setLines(QVector<QGraphicsLineItem*> *vector);
    void setTableWidget(QTableWidget *widget);
    void setTableMutex(QMutex *mutex);
    void setScene(QGraphicsScene * s);

signals:

private:
    // Bounding rectangle parameters
    const int rectX = -5;
    const int rectY = -5;
    const int rectWidth = 10;
    const int rectHeight = 10;

    QGraphicsLineItem *inLine;
    QGraphicsLineItem *outLine;
    QVector<std::shared_ptr<MoveItem> > *points;
    QVector<QGraphicsLineItem*> *lines;
    QTableWidget *tableWidget;
    QMutex * tableMutex;
    QGraphicsScene *parScene;

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    int getPos();

public slots:
    void deletePoint(QAction *action);
};

#endif // MOVEITEM_H
