#include "moveitem.h"
#include <iostream>

qreal toCoordinates(qreal value, bool axis) {
    if (axis) {
        return value - 250;
    }
    else {
        return 500 - value - 250;
    }
}

qreal fromCoordinates(qreal value, bool axis) {
    if (axis) {
        return value + 250;
    }
    else {
        return (500 - value) - 250;
    }
}

MoveItem::MoveItem(QObject *parent) :
    QObject(parent), QGraphicsItem()
{
    inLine = nullptr;
    outLine = nullptr;
}

MoveItem::~MoveItem()
{

}

void MoveItem::setInLine(QGraphicsLineItem *in) {
    inLine = in;
}

void MoveItem::setOutLine(QGraphicsLineItem *out) {
    outLine = out;
}

void MoveItem::setPoints(QVector<std::shared_ptr<MoveItem> > * vector) {
    points = vector;
}

void MoveItem::setLines(QVector<QGraphicsLineItem *> *vector) {
    lines = vector;
}

void MoveItem::setTableWidget(QTableWidget *widget){
    tableWidget = widget;
}

void MoveItem::setTableMutex(QMutex *mutex) {
    tableMutex = mutex;
}

void MoveItem::setScene(QGraphicsScene *s) {
    parScene = s;
}

QRectF MoveItem::boundingRect() const
{
    return QRectF(rectX, rectY, rectWidth, rectHeight);
}

void MoveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::black);
    painter->setBrush(Qt::blue);
    painter->drawEllipse(rectX, rectY, rectWidth, rectHeight);
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

void MoveItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    tableMutex->lock();
    // redraw lines
    this->setPos(mapToScene(event->pos()));
    if (inLine != nullptr) {
        QLineF line = inLine->line();
        inLine->setLine(QLineF(line.p1(), mapToScene(event->pos())));
    }
    if (outLine != nullptr) {
        QLineF line = outLine->line();
        outLine->setLine(QLineF(mapToScene(event->pos()), line.p2()));
    }

    // rewrite data in table
    int index = getPos();
    tableWidget->setItem(index, 0, new QTableWidgetItem(tr(std::to_string(toCoordinates(this->x(), true)).c_str())));
    tableWidget->setItem(index, 1, new QTableWidgetItem(tr(std::to_string(toCoordinates(this->y(), false)).c_str())));
    tableMutex->unlock();
}

void MoveItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // call menu
    if (event->button() == Qt::RightButton) {
        QMenu* menu = new QMenu();

        menu->addAction("Удалить точку");
        menu->popup(QCursor::pos());
        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(deletePoint(QAction*)));
    }
    this->setCursor(QCursor(Qt::ClosedHandCursor));
}

void MoveItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event)
}

void MoveItem::deletePoint(QAction *action) {
    if (action->text() == "Удалить точку") {
        int index = getPos();

        if (index > 0 && index+1 < points->size()) {
            QPointF newEnd = outLine->line().p2();
            QPointF newBegin = inLine->line().p1();

            QGraphicsLineItem *newLine = parScene->addLine(newBegin.x(), newBegin.y(), newEnd.x(), newEnd.y(), QPen(Qt::blue, 2));

            points->at(index-1)->setOutLine(newLine);
            points->at(index+1)->setInLine(newLine);

            delete outLine;
            delete inLine;

            lines->replace(index-1, newLine);
            lines->remove(index);
        }
        else if (index == 0) {
            delete outLine;
            points->at(index+1)->setInLine(nullptr);
            lines->removeFirst();
        } else {
            delete inLine;
            points->at(index-1)->setOutLine(nullptr);
            lines->removeLast();
        }

        tableWidget->removeRow(index);
        points->remove(index);
    }
}

int MoveItem::getPos() {
    int index = 0;
    for (int i = 0; i < points->size(); i++) {
        if (points->at(i).get() == this) {
            index = i;
        }
    }
    return index;
}
