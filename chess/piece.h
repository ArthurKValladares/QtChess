#ifndef PIECE_H
#define PIECE_H

#include <QGraphicsPixmapItem>
#include "tile.h"
#include <QGraphicsSceneMouseEvent>

enum codes {
    b_king, b_queen, b_rook, b_bishop, b_knight, b_pawn,
    w_king, w_queen, w_rook, w_bishop, w_knight, w_pawn,
    t_empty
};

class Tile;
class Piece:public QGraphicsPixmapItem
{
public:
    Piece(QString team = "",  uint8_t code = t_empty, QGraphicsItem *parent = 0);

    // Set-up functions
    void set_tile(Tile *tile);
    void reset_tile_color();
    bool set_tile_color(Tile *box);
    void setSide( QString value);
    void link_image();
    void upgrade();
    void revert_upgrade();

    // Move logic
    void is_move_valid(int r, int c);
    void check_line(int change_row, int change_col);
    void get_valid_moves();
    void find_invalid_locations();


    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    bool is_first_move;
    uint8_t code;
    Tile *curr_tile;
    QList <Tile *> possible_moves;
    QString side;
    bool on_board;
};

#endif // CHESSPIECE_H
