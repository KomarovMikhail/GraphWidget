#include "widget.h"
#include "ui_widget.h"

static int randomBetween(int low, int high)
{
    return (qrand() % ((high + 1) - low) + low);
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->resize(840, 640);
    this->setFixedSize(840, 640);

    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    ui->graphicsView->resize(600, 600);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    ui->tableWidget->resize(200, 600);
    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->setColumnCount(2);

    QStringList headerLabels;
    headerLabels << "X" << "Y";
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    scene->setSceneRect(0, 0, 500, 500);

    QGraphicsLineItem *line = scene->addLine(250, -1000, 250, 1000);
    line->setZValue(-10);
    line = scene->addLine(-1000, 250, 1000, 250);
    line->setZValue(-10);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    std::shared_ptr<MoveItem> item = std::make_shared<MoveItem>(this);
    item->setPos(randomBetween(30, 470),
                 randomBetween(30, 470));

    // draw new point and line
    scene->addItem(item.get());

    if (!points.empty()) {
        std::shared_ptr<MoveItem>last = points.last();
        QGraphicsLineItem *line = scene->addLine(last->x(), last->y(), item->x(), item->y(), QPen(Qt::blue, 2));
        line->setZValue(-1);
        lines.push_back(line);

        last->setOutLine(line);
        item->setInLine(line);
    }
    item->setPoints(&points);
    item->setTableWidget(ui->tableWidget);
    item->setTableMutex(&tableMutex);
    item->setScene(scene);
    item->setLines(&lines);
    points.push_back(item);

    // add point data to table
    tableMutex.lock();
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(tr(std::to_string(toCoordinates(item->x(), true)).c_str())));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(tr(std::to_string(toCoordinates(item->y(), false)).c_str())));
    tableMutex.unlock();
}

void Widget::on_tableWidget_cellChanged(int row, int column) {
    if (!tableMutex.tryLock()) {
        return;
    }

    QTableWidgetItem * item = ui->tableWidget->item(row, column);
    bool ok;
    qreal value = fromCoordinates(item->text().toLong(&ok), !column);

    if (!ok) {
        QMessageBox msgBox;
        msgBox.setText("The value is incorrect");
        msgBox.exec();
        tableMutex.unlock();
        return;
    }

    if (column == 0) {
        points[row]->setX(value);
        if (row < lines.size()) {
            QLineF line = lines[row]->line();
            lines[row]->setLine(value, line.y1(), line.x2(), line.y2());
        }
        if (row > 0) {
            QLineF line = lines[row-1]->line();
            lines[row-1]->setLine(line.x1(), line.y1(), value, line.y2());
        }
    }
    if (column == 1) {
        points[row]->setY(value);
        if (row < lines.size()) {
            QLineF line = lines[row]->line();
            lines[row]->setLine(line.x1(), value, line.x2(), line.y2());
        }
        if (row > 0) {
            QLineF line = lines[row-1]->line();
            lines[row-1]->setLine(line.x1(), line.y1(), line.x2(), value);
        }
    }

    tableMutex.unlock();
}
