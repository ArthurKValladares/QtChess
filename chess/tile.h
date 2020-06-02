#ifndef TILE_H
#define TILE_H

#include <QGraphicsRectItem>
#include <QBrush>
#include "piece.h"
#include <QGraphicsSceneMouseEvent>

class Piece;
class Tile:public QGraphicsRectItem
{
public:
    Tile(QGraphicsItem *parent=0);
    ~Tile();

    // Color functions
    void set_color(QColor color);
    void reset_color();
    void set_original_color(QColor value);

    // Piece realated functions
    void place_piece(Piece *piece);
    void set_has_piece(bool value, Piece *piece = 0);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    // Public members
    int row;
    int col;
    Piece * curr_piece;
    QString piece_color;
    bool has_piece;
private:
    QBrush brush;
    QColor color_original;
};

#endif // CHESSBOX_H
