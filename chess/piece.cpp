#include "piece.h"
#include "game.h"
#include <QDebug>

extern Game *game;

// General piece functions
Piece::Piece(QString team,  uint8_t c, QGraphicsItem *parent):QGraphicsPixmapItem(parent) {
    side = team;
    on_board = true;
    is_first_move = true;
    code = c;
    link_image();
}

void Piece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    //if it is already consumed or not the respective color's turn
    if((!on_board)|| ( (game->turn != this->side)&& (!game->moving_piece)) )
        return;
    //selecting
    if(!game->moving_piece){
        game->moving_piece = this;
        game->moving_piece->get_valid_moves();
        if (possible_moves.empty()) {
            // TODO: FLASH RED FOR A SECOND HERE
            game->moving_piece = NULL;
        } else {
            game->moving_piece->curr_tile->set_color(QColor(39, 214, 85));
        }
    }
    //Consuming counterPart of the CHessBox
    else if(this->side != game->moving_piece->side){
        this->curr_tile->mousePressEvent(event);
    }
}

void Piece::set_tile(Tile *tile) {

    curr_tile = tile;
}

void Piece::setSide( QString value) {
    side = value;
}

void Piece::link_image() {
    QString file_paths[] = {":/pieces/images/black_king.png",
                            ":/pieces/images/black_queen.png",
                            ":/pieces/images/black_rook.png",
                            ":/pieces/images/black_bishop.png",
                            ":/pieces/images/black_knight.png",
                            ":/pieces/images/black_pawn.png",
                            ":/pieces/images/white_king.png",
                            ":/pieces/images/white_queen.png",
                            ":/pieces/images/white_rook.png",
                            ":/pieces/images/white_bishop.png",
                            ":/pieces/images/white_knight.png",
                            ":/pieces/images/white_pawn.png"};
    QPixmap tile_img = QPixmap(file_paths[code]);
    setPixmap(tile_img);
}

void Piece::is_move_valid(int r, int c) {
    //QString pside = side;
    // Save previous piece in tile we are moving to
    Piece *prev_piece = game->game_board[r][c]->curr_piece;
    int prev_r = curr_tile->row, prev_c = curr_tile->col;
    // set piece in that tile to this, set piece in current tile to null
    game->game_board[r][c]->curr_piece = this;
    curr_tile->curr_piece = nullptr;
    curr_tile->row = r;
    curr_tile->col = c;
    bool in_check = game->is_in_check(side);
    // Revert changes
    curr_tile->curr_piece = this;
    curr_tile->row = prev_r;
    curr_tile->col = prev_c;
    game->game_board[r][c]->curr_piece = prev_piece;
    if (!in_check) {
        possible_moves.append(game->game_board[r][c]);
    }
}

void Piece::check_line(int change_row, int change_col) {
    int limit_row = change_row > 0 ? 8:-1;
    int limit_col = change_col > 0 ? 8:-1;
    for (int r = this->curr_tile->row + change_row, c = this->curr_tile->col + change_col;
         r != limit_row && c != limit_col;
         r += change_row, c+= change_col) {
        if (game->game_board[r][c]->piece_color == this->side) {
            break;
        } else {
            is_move_valid(r, c);
            if (!possible_moves.empty() && set_tile_color(possible_moves.last())) {
                break;
            }
        }
    }
}


void Piece::get_valid_moves()
{
    possible_moves.clear();
    int row = this->curr_tile->row;
    int col = this->curr_tile->col;
    QString team = this->side;
    // Hadling moves for pawns
    if (code == w_pawn || code == b_pawn) {
        int8_t row_change = (code == w_pawn) ? -1:+1;
        uint8_t row_limit = (code == w_pawn) ? 0:7;
        // Test if we can attack to the left
        if (col > 0 && row != row_limit &&
            game->game_board[row + row_change][col-1]->has_piece &&
            game->game_board[row + row_change][col-1]->piece_color != team) {
                is_move_valid(row+row_change, col-1);
                //location.append(game->collection[row+row_change][col-1]);
                if (!possible_moves.empty()) {
                    set_tile_color(possible_moves.last());
                }
        }
        // Test if we can attack to the right
        if (col < 7 && row != row_limit &&
            game->game_board[row + row_change][col+1]->has_piece &&
            game->game_board[row + row_change][col+1]->piece_color != team) {
                //location.append(game->collection[row+row_change][col+1]);
                is_move_valid(row+row_change, col+1);
                if (!possible_moves.empty()) {
                    set_tile_color(possible_moves.last());
                }
        }
        // Test if we can move forwards
        if (row != row_limit && (!game->game_board[row+row_change][col]->has_piece)) {
            //location.append(game->collection[row+row_change][col]);
            is_move_valid(row+row_change, col);
            if (!possible_moves.empty()) {
                set_tile_color(possible_moves.last());
            }
            // TODO: For second move, do I need to check if first move is possible?
            if (is_first_move && !game->game_board[row+2*row_change][col]->has_piece) {
                //location.append(game->collection[row+2*row_change][col]);
                is_move_valid(row+2*row_change, col);
                if (!possible_moves.empty()) {
                    set_tile_color(possible_moves.last());
                }
            }
        }
    } else if (code == w_rook || code == b_rook) {
        check_line(1, 0);
        check_line(0, 1);
        check_line(0, -1);
        check_line(-1, 0);
    } else if (code == w_bishop || code == b_bishop) {
        check_line(1, 1);
        check_line(1, -1);
        check_line(-1, 1);
        check_line(-1, -1);
    } else if (code == w_queen || code == b_queen) {
        check_line(1, 0);
        check_line(0, 1);
        check_line(0, -1);
        check_line(-1, 0);
        check_line(1, 1);
        check_line(1, -1);
        check_line(-1, 1);
        check_line(-1, -1);
    } else if (code == w_knight || code == b_knight) {
        QVector<QPair<int, int>> moves = {{row + 2, col + 1}, {row + 2, col - 1}, {row + 1, col + 2}, {row + 1, col - 2},
                                          {row - 2, col + 1}, {row - 2, col - 1}, {row - 1, col + 2}, {row - 1, col - 2}};
        for (size_t i = 0; i < 8; ++i) {
            const int n_row = moves[i].first;
            const int n_col = moves[i].second;
            if (n_row >= 0 && n_row <= 7 && n_col >= 0 && n_col <= 7 &&
                game->game_board[n_row][n_col]->piece_color != team) {
                is_move_valid(n_row, n_col);
                if (!possible_moves.empty()) {
                    set_tile_color(possible_moves.last());
                }
            }
        }
    } else if (code == w_king || code == b_king) {
        QVector<QPair<int, int>> moves = {{row + 1, col + 1}, {row + 1, col}, {row + 1, col - 1},
                                          {row, col + 1}, {row, col - 1},
                                          {row - 1, col + 1}, {row - 1, col}, {row - 1, col - 1}};
        for (size_t i = 0; i < 8; ++i) {
            const int n_row = moves[i].first;
            const int n_col = moves[i].second;
            if (n_row >= 0 && n_row <= 7 && n_col >= 0 && n_col <= 7 &&
                game->game_board[n_row][n_col]->piece_color != team) {
                is_move_valid(n_row, n_col);
                if (!possible_moves.empty()) {
                    set_tile_color(possible_moves.last());
                }
            }
        }
    }
}

void Piece::reset_tile_color()
{
    for(size_t i = 0, n = possible_moves.size(); i < n;i++) {
         possible_moves[i]->reset_color();
    }
}

bool Piece::set_tile_color(Tile *box)
{
    if(box->has_piece) {
        Piece *q = dynamic_cast<Piece*>(possible_moves.last()->curr_piece);
        if(q && (q->code == w_king || q->code == b_king)){
            box->set_color(Qt::blue);
        }
        else {
            box->set_color(QColor(255, 74, 74));
        }
        return true;
    }
    else
        possible_moves.last()->set_color(QColor(243, 255, 115));
    return false;
}

void Piece::upgrade() {
    if (code == b_pawn) {
        code = b_queen;
    } else if (code == w_pawn) {
        code = w_queen;
    }
}

void Piece::revert_upgrade() {
    if (code == b_queen) {
        code = b_pawn;
    } else if (code == w_queen) {
        code = w_pawn;
    }
}
