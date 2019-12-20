#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsLineItem>
#include <QMutex>
#include <QMessageBox>
#include <memory>

#include <moveitem.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void on_tableWidget_cellChanged(int row, int column);

private:
    Ui::Widget *ui;
    QGraphicsScene *scene;

    QVector<std::shared_ptr<MoveItem> > points;
    QVector<QGraphicsLineItem*> lines;

    QMutex tableMutex;

};

#endif // WIDGET_H
