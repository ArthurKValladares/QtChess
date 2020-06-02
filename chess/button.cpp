#include "button.h"
#include <QGraphicsTextItem>
#include <QBrush>

Button::Button(QString name, int x, int y, QGraphicsItem *parent):QGraphicsRectItem(parent) {
    // Set up Button box
    setRect(0,0,160,60);
    setPos(x + 20, y + 20);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(119, 149, 86));
    setBrush(brush);
    //Setup Button text
    button_text = new QGraphicsTextItem(name,this);
    button_text->setPos(rect().width()/2 - button_text->boundingRect().width()/2,
                        rect().height()/2 - button_text->boundingRect().height()/2);
    button_text->setDefaultTextColor(Qt::white);

    setAcceptHoverEvents(true);
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(event) {
        emit clicked();
    }
}

void Button::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(event){
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(QColor(79, 109, 46));
        setBrush(brush);
    }
}

void Button::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    if(event){
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(QColor(119, 149, 86));
        setBrush(brush);
    }
}
