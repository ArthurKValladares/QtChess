#include "board.h"
#include "tile.h"
#include "game.h"
#include "piece.h"

extern Game *game;

Board::Board() {
    set_up_black_side();
    set_up_white_side();
}

void Board::draw_tiles() {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            Tile *box = new Tile();
            game->game_board[i][j] = box;
            box->row = i;
            box->col = j;
            box->setPos(TILE_WIDTH*(2+j),TILE_WIDTH*(1+i));
            if((i+j)%2==0) {
                box->set_original_color(QColor(235, 236, 208));
            } else {
                box->set_original_color(QColor(119, 149, 86));
            }
            game->add_to_scene(box);
        }
    }
}

void Board::add_piece() {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            Tile *box =game->game_board[i][j];
            if(i < 2) {
                static int k;
                box->place_piece(black[k]);
                game->alive_pieces.append(black[k]);
                game->add_to_scene(black[k++]);
            }
            if(i > 5) {
                static int h;
                box->place_piece(white[h]);
                game->alive_pieces.append(white[h]);
                game->add_to_scene(white[h++]);
            }
        }
    }
}

void Board::set_up_white_side() {
    Piece *piece;
    for(int i = 0; i < 8; i++) {
        piece = new Piece("WHITE", w_pawn);
        white.append(piece);
    }
    piece = new Piece("WHITE", w_rook);
    white.append(piece);
    piece = new Piece("WHITE", w_knight);
    white.append(piece);
    piece = new Piece("WHITE", w_bishop);
    white.append(piece);
    piece = new Piece("WHITE", w_queen);
    white.append(piece);
    piece = new Piece("WHITE", w_king);
    white.append(piece);
    king_w = white.back();
    piece = new Piece("WHITE", w_bishop);
    white.append(piece);
    piece = new Piece("WHITE", w_knight);
    white.append(piece);
    piece = new Piece("WHITE", w_rook);
    white.append(piece);
}

void Board::set_up_black_side()
{
    Piece *piece;
    piece = new Piece("BLACK", b_rook);
    black.append(piece);
    piece = new Piece("BLACK", b_knight);
    black.append(piece);
    piece = new Piece("BLACK", b_bishop);
    black.append(piece);
    piece = new Piece("BLACK", b_queen);
    black.append(piece);
    piece = new Piece("BLACK", b_king);
    black.append(piece);
    king_b = black.back();
    piece = new Piece("BLACK", b_bishop);
    black.append(piece);
    piece = new Piece("BLACK", b_knight);
    black.append(piece);
    piece = new Piece("BLACK", b_rook);
    black.append(piece);
    for(int i = 0; i < 8; i++) {
        piece = new Piece("BLACK", b_pawn);
        black.append(piece);
    }
}

void Board::reset() {
    int k = 0; int h = 0;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++){
            Tile *box = game->game_board[i][j];
            box->set_has_piece(false);
            box->piece_color = "NONE";
            box->curr_piece = NULL;
            if(i < 2) {
                box->place_piece(black[k]);
                black[k]->on_board = true;
                black[k]->is_first_move = true;
                game->alive_pieces.append(black[k++]);
            }
            if(i > 5) {
                box->place_piece(white[h]);
                white[h]->on_board = true;
                white[h]->is_first_move = true;
                game->alive_pieces.append(white[h++]);

            }

        }
    }
}
