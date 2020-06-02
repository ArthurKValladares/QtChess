#ifndef BOARD_H
#define BOARD_H
#include <QGraphicsRectItem>
#include "piece.h"
class Board
{
public:
    Board();

    // "Graphics" functions
    void draw_tiles();

    // Set-up/Clean-up functions
    void set_up_white_side();
    void set_up_black_side();
    void reset();
    void add_piece();

    // Game Logic
    void is_in_check(bool is_white);

    // Public members
    Piece *king_w;
    Piece *king_b;
private:
    QList <Piece *> white;
    QList <Piece *> black;
};

#endif // CHESSBOARD_H
