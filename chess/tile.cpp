#include "tile.h"
#include "game.h"
#include <QDebug>
#include "piece.h"

extern Game *game;
Tile::Tile(QGraphicsItem *parent):QGraphicsRectItem(parent) {
    //making the Square CHess Box
    setRect(0,0,100,100);
    brush.setStyle(Qt::SolidPattern);
    setZValue(-1);
    set_has_piece(false);
    piece_color = "NONE";
    curr_piece = NULL;
}

Tile::~Tile() {
    delete this;
}

void Tile::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(curr_piece == game->moving_piece && curr_piece){
        curr_piece->mousePressEvent(event);
        return;
    }
    //if selected
    if(game->moving_piece){
        //if same team
        if(this->piece_color == game->moving_piece->side)
            return;
        //removing the eaten piece
        QList <Tile *> movLoc = game->moving_piece->possible_moves;
        //To make sure the selected box is in move zone
        int check = 0;
        for(size_t i = 0, n = movLoc.size(); i < n;i++) {
            if(movLoc[i] == this) {
                check++;
            }
        }
        // if not present return
        if(check == 0)
            return;
        //change the color back to normal
         game->moving_piece->reset_tile_color();
         //make the first move false applicable for pawn only
         game->moving_piece->is_first_move = false;
         //this is to eat or consume the enemy present in the movable region
        if(this->has_piece){
            this->curr_piece->on_board = false;
            this->curr_piece->set_tile(NULL);
            game->put_in_cemetary(this->curr_piece);

        }
        //changing the new stat and resetting the previous left region
        game->moving_piece->curr_tile->set_has_piece(false);
        game->moving_piece->curr_tile->piece_color = "NONE";
        game->moving_piece->curr_tile->curr_piece = NULL;
        game->moving_piece->curr_tile->reset_color();
        place_piece(game->moving_piece);
        // Upgrade Pawn if it reached last row
        if ((row == 0 && game->moving_piece->code == w_pawn) ||
            (row == 7 && game->moving_piece->code == b_pawn)) {
            game->moving_piece->upgrade();
            game->moving_piece->link_image();
        }
        //changing turn
        game->moving_piece = NULL;
        game->swicth_turns();
        if (game->is_in_check(game->turn)) {
            game->check_display->setVisible(true);
            if (game->is_check_mate(game->turn)) {
                game->game_over();
                return;
            }
        } else {
            game->check_display->setVisible(false);
        }
        if (game->vsAI && game->turn == "BLACK") {
            game->make_move_AI();
        }
    }
    else if(this->has_piece)
    {
        this->curr_piece->mousePressEvent(event);
    }
}

void Tile::set_color(QColor color) {
    if (game->vsAI && game->turn == "BLACK") {
        return;
    }
    brush.setColor(color);
    setBrush(color);
}

void Tile::place_piece(Piece *piece) {
    piece->setPos(x()+50- piece->pixmap().width()/2  ,y()+50-piece->pixmap().width()/2);
    piece->set_tile(this);
    set_has_piece(true,piece);
    curr_piece = piece;
}

void Tile::reset_color() {
    set_color(color_original);
}

void Tile::set_original_color(QColor value) {
    color_original = value;
    set_color(color_original);
}

void Tile::set_has_piece(bool value, Piece *piece) {
    has_piece = value;
    if(value) {
        piece_color = piece->side;
    } else {
        piece_color = "NONE";
    }
}
