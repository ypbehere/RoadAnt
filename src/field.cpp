#include <QDebug>
#include "field.h"
namespace{
    const static QColor COLOR_BLUE(210,210,255);
    const static QColor COLOR_YELLOW(241,231,36);
    const static QColor COLOR_PINK(255,63,149);
    const static QColor COLOR_GREEN(105,255,0);
    const static QColor COLOR_ORANGE(255,170,85);
}

Field::Field(QQuickItem *parent): QQuickPaintedItem(parent), pixmap(nullptr), pen(Qt::white,1) {
    connect(City::instance(),SIGNAL(needDraw()),this,SLOT(draw()));
    pixmap = new QPixmap(QSize(640, 480));
    pixmapPainter.begin(pixmap);
    area = QRect(0,0,640,480);
}

void Field::draw() {
    pixmap->fill(COLOR_BLUE);
    pixmapPainter.strokePath(painterPath, pen);
    fillField();
    this->update(area);
}

void Field::paint(QPainter* painter){
    painter->drawPixmap(area,*pixmap);
}

void Field::paintRoad(const QColor &color, QPointF start, QPointF end) {
    pixmapPainter.setBrush(QBrush(color));
    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.drawLine(start, end);
}

void Field::paintStore(const QColor &color, qreal x, qreal y) {
    static float radius = 5;
    pixmapPainter.setBrush(QBrush(color));
    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.drawEllipse(x - radius, y - radius,2 * radius, 2 * radius);
}

void Field::paintDriver(const QColor &color, qreal x, qreal y) {
    static float radius = 5;
    pixmapPainter.setBrush(QBrush(color));
    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.drawEllipse(x - radius, y - radius,2 * radius, 2 * radius);
}

void Field::fillField() {
    for (int i = 0; i < City::instance()->roadNum(); i++) {
        CPos start = City::instance()->road(i).start();
        CPos end = City::instance()->road(i).end();
        QPointF qstart(start._x, start._y);
        QPointF qend(end._x, end._y);
        paintRoad(COLOR_ORANGE, qstart, qend);
    }
    for (int i = 0; i < City::instance()->driverNum(); i++) {
        paintDriver(COLOR_BLUE, City::instance()->driver(i).pos()._x, City::instance()->driver(i).pos()._y);
    }
    for (int i = 0; i < City::instance()->storeNum(); i++) {
        paintStore(COLOR_YELLOW, City::instance()->store(i).pos()._x, City::instance()->store(i).pos()._y);
    }
}
