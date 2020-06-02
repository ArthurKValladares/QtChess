#include "game.h"
#include "button.h"
#include <QPixmap>
#include "piece.h"
#include <QDebug>

bool is_valid(int r, int c) {
    return (r >= 0 && r <= 7 && c >= 0 && c <= 7);
}

inline int max_(int x, int y) {
    return x > y ? x : y;
}

inline int min_(int x, int y) {
    return x < y ? x : y;
}

Game::Game(QWidget *parent ):QGraphicsView(parent) {
    //Making the Scene
    gameScene = new QGraphicsScene();
    gameScene->setSceneRect(0,0,TILE_WIDTH*12,TILE_WIDTH*10);

    //Making the view
    setFixedSize(TILE_WIDTH*12,TILE_WIDTH*10);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(gameScene);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(60, 61, 56));
    setBackgroundBrush(brush);
    moving_piece = NULL;

    //display turn
    turnDisplay = new QGraphicsTextItem();
    turnDisplay->setZValue(1);
    turnDisplay->setDefaultTextColor(Qt::white);
    turnDisplay->setFont(QFont("",18));
    turnDisplay->setPlainText("WHITE TURN");
    turnDisplay->setPos(TILE_WIDTH*2 + (TILE_WIDTH*8 - turnDisplay->boundingRect().width())/2,
                       (TILE_WIDTH - turnDisplay->boundingRect().height())/2);
    turn = "WHITE";

    //display Check
    check_display = new QGraphicsTextItem();
    check_display->setZValue(4);
    check_display->setDefaultTextColor(QColor(255, 38, 0));
    check_display->setFont(QFont("Helvetica" , 18));
    check_display->setPlainText("CHECK!");
    check_display->setVisible(false);
    check_display->setPos(TILE_WIDTH*2 + (TILE_WIDTH*8 - check_display->boundingRect().width())/2,
                  TILE_WIDTH*9 + (TILE_WIDTH - turnDisplay->boundingRect().height())/2);
}

void Game::draw_board() {
    chess = new Board();
    QColor dead_color = QColor(87, 84, 82);
    draw_cemetary(0,0,dead_color);
    draw_cemetary(TILE_WIDTH*10,0,dead_color);
    chess->draw_tiles();
}

void Game::draw_dead_pieces(bool is_white) {
    int j = 0;
    int k = 0;
    QList<Piece *> &dead_list = is_white ? whiteDead :  blackDead;
    const int offset = is_white ? 0 : TILE_WIDTH*10;
    for(size_t i = 0,n = dead_list.size(); i<n; i++) {
        if(k == 8){
            j++;
            k = 0;
        }
        dead_list[i]->setPos(offset+TILE_WIDTH*j,TILE_WIDTH*++k);
     }
}

void Game::put_in_cemetary(Piece *piece) {
    bool is_white = (piece->side == "WHITE");
    QList<Piece *> &dead_list =  is_white ? whiteDead:blackDead;
    dead_list.append(piece);
    draw_dead_pieces(is_white);
    alive_pieces.removeAll(piece);
}

void Game::add_to_scene(QGraphicsItem *item) {
    gameScene->addItem(item);
}

void Game::remove_from_scene(QGraphicsItem *item) {
    gameScene->removeItem(item);

}

void Game::swicth_turns() {
    QColor color;
    if(turn == "WHITE") {
        turn = "BLACK";
        color = Qt::black;
    } else {
        turn = "WHITE";
        color = Qt::white;
    }
    turnDisplay->setPlainText(turn + " TURN");
    turnDisplay->setDefaultTextColor(color);
}

int Game::piece_value(char code, size_t r, size_t c) {
    if(code == t_empty) {
        return 0;
    } else {
        int p_values[13] = {20000, 900, 500, 330, 320, 100};
        // values taken from https://www.chessprogramming.org/Simplified_Evaluation_Function
        int pawn_values[8][8] = {{900,  900,  900,  900,  900,  900,  900,  900},
                                 {50, 50, 50, 50, 50, 50, 50, 50},
                                 {10, 10, 20, 30, 30, 20, 10, 10},
                                 {5,  5, 10, 25, 25, 10,  5,  5},
                                 {0,  0,  0, 20, 20,  0,  0,  0},
                                 {5, -5,-10,  0,  0,-10, -5,  5},
                                 {5, 10, 10,-20,-20, 10, 10,  5},
                                 {0,  0,  0,  0,  0,  0,  0,  0}};

        int knight_values[8][8] = {{-50,-40,-30,-30,-30,-30,-40,-50},
                                   {-40,-20,  0,  0,  0,  0,-20,-40},
                                   {-30,  0, 10, 15, 15, 10,  0,-30},
                                   {-30,  5, 15, 20, 20, 15,  5,-30},
                                   {-30,  0, 15, 20, 20, 15,  0,-30},
                                   {-30,  5, 10, 15, 15, 10,  5,-30},
                                   {-40,-20,  0,  5,  5,  0,-20,-40},
                                   {-50,-40,-30,-30,-30,-30,-40,-50}};

        int bishop_values[8][8] = {{-20,-10,-10,-10,-10,-10,-10,-20},
                                   {-10,  0,  0,  0,  0,  0,  0,-10},
                                   {-10,  0,  5, 10, 10,  5,  0,-10},
                                   {-10,  5,  5, 10, 10,  5,  5,-10},
                                   {-10,  0, 10, 10, 10, 10,  0,-10},
                                   {-10, 10, 10, 10, 10, 10, 10,-10},
                                   {-10,  5,  0,  0,  0,  0,  5,-10},
                                   {-20,-10,-10,-10,-10,-10,-10,-20}};

        int rook_values[8][8] = {{0,  0,  0,  0,  0,  0,  0,  0},
                                 {5, 10, 10, 10, 10, 10, 10,  5},
                                 {-5,  0,  0,  0,  0,  0,  0, -5},
                                 {-5,  0,  0,  0,  0,  0,  0, -5},
                                 {-5,  0,  0,  0,  0,  0,  0, -5},
                                 {-5,  0,  0,  0,  0,  0,  0, -5},
                                 {-5,  0,  0,  0,  0,  0,  0, -5},
                                 {0,  0,  0,  5,  5,  0,  0,  0}};

        int queen_values[8][8] = {{-20,-10,-10, -5, -5,-10,-10,-20},
                                  {-10,  0,  0,  0,  0,  0,  0,-10},
                                  {-10,  0,  5,  5,  5,  5,  0,-10},
                                  {-5,  0,  5,  5,  5,  5,  0, -5},
                                  {0,  0,  5,  5,  5,  5,  0, -5},
                                  {-10,  5,  5,  5,  5,  5,  0,-10},
                                  {-10,  0,  5,  0,  0,  0,  0,-10},
                                  {-20,-10,-10, -5, -5,-10,-10,-20}};

        int king_values[8][8] = {{-30,-40,-40,-50,-50,-40,-40,-30},
                                 {-30,-40,-40,-50,-50,-40,-40,-30},
                                 {-30,-40,-40,-50,-50,-40,-40,-30},
                                 {-30,-40,-40,-50,-50,-40,-40,-30},
                                 {-20,-30,-30,-40,-40,-30,-30,-20},
                                 {-10,-20,-20,-20,-20,-20,-20,-10},
                                 {20, 20,  0,  0,  0,  0, 20, 20},
                                 {20, 30, 10,  0,  0, 10, 30, 20}};

        int piece_val = p_values[code % 6];
        if (code == b_king || code == w_king) {
            int n_r = (code == w_king) ? r : 7 - r;
            piece_val += king_values[n_r][c];
        } else if (code == b_queen || code == w_queen) {
            int n_r = (code == w_queen) ? r : 7 - r;
            piece_val += queen_values[n_r][c];
        } else if (code == b_rook || code == w_rook) {
            int n_r = (code == w_rook) ? r : 7 - r;
            piece_val += rook_values[n_r][c];
        } else if (code == b_knight || code == w_knight) {
            int n_r = (code == w_knight) ? r : 7 - r;
            piece_val += knight_values[n_r][c];
        } else if (code == b_pawn || code == w_pawn) {
            int n_r = (code == w_pawn) ? r : 7 - r;
            piece_val += pawn_values[n_r][c];
        } else if (code == b_bishop || code == w_bishop) {
            int n_r = (code == w_pawn) ? r : 7 - r;
            piece_val += bishop_values[n_r][c];
        }
        if (code < 6) {
            piece_val = piece_val * (-1);
        }
        return piece_val;
    }
}

int Game::evaluation_function() {
    int board_value = 0;
    for (size_t i = 0; i < alive_pieces.size(); ++i) {
        if (alive_pieces[i]->curr_tile != nullptr) {
            board_value += piece_value(alive_pieces[i]->code, alive_pieces[i]->curr_tile->row, alive_pieces[i]->curr_tile->col);
        }
    }
    return board_value;
}

int Game::minmax(int depth, const int max_depth, QString side, int alpha, int beta, Piece **best_piece, Tile **best_pos) {
    if (depth == max_depth) {
        return evaluation_function();
    }
    int best_val;
    if (side == "WHITE") {
        best_val = -1000000;
        for (auto &p : alive_pieces) {
            if(p->side == "WHITE" && p->curr_tile != nullptr) {
                p->get_valid_moves();
                QList <Tile *> moves = p->possible_moves;
                for (auto &l : moves) {
                    // Temporarily execute move
                    Tile *p_box = p->curr_tile;
                    Piece *l_piece = l->curr_piece;

                    p->curr_tile = l;
                    l->curr_piece = p;
                    p_box->curr_piece = nullptr;
                    if (l_piece != nullptr) {
                        l_piece->curr_tile = nullptr;
                    }
                    if (l->row == 7 && p->code == b_pawn || l->row == 0 && p->code == w_pawn) {
                        p->upgrade();
                    }
                    //  move deeper
                    int val = minmax(depth + 1, max_depth, "BLACK", alpha, beta, best_piece, best_pos);
                    // Revert move
                    if (l->row == 7 && p->code == b_pawn || l->row == 0 && p->code == w_pawn) {
                        p->revert_upgrade();
                    }
                    p->curr_tile = p_box;
                    p_box->curr_piece = p;
                    if (l_piece != nullptr) {
                        l_piece->curr_tile = l;
                    }
                    l->curr_piece = l_piece;

                    if (val >= best_val) {
                        best_val = val;
                        if (depth == 0) {
                            *best_pos = l;
                            *best_piece = p;
                        }
                    }
                    alpha = max_(alpha, best_val);
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
        return best_val;
    } else {
        best_val = 1000000;
        for (auto &p : alive_pieces) {
            if(p->side == "BLACK" && p->curr_tile != nullptr) {
                p->get_valid_moves();
                QList <Tile *> moves = p->possible_moves;
                for (auto &l:moves) {
                    // Temporarily execute move
                    Tile *p_box = p->curr_tile;
                    Piece *l_piece = l->curr_piece;

                    p->curr_tile = l;
                    l->curr_piece = p;
                    p_box->curr_piece = nullptr;
                    if (l_piece != nullptr) {
                        l_piece->curr_tile = nullptr;
                    }
                    if (l->row == 7 && p->code == b_pawn || l->row == 0 && p->code == w_pawn) {
                        p->upgrade();
                    }
                    //  move deeper
                    int val = minmax(depth + 1, max_depth, "WHITE", alpha, beta, best_piece, best_pos);
                    if (l->row == 7 && p->code == b_pawn || l->row == 0 && p->code == w_pawn) {
                        p->revert_upgrade();
                    }
                    // Revert move
                    p->curr_tile = p_box;
                    p_box->curr_piece = p;
                    if (l_piece != nullptr) {
                        l_piece->curr_tile = l;
                    }
                    l->curr_piece = l_piece;

                    if (val <= best_val) {
                        best_val = val;
                        if (depth == 0) {
                            *best_pos = l;
                            *best_piece = p;
                        }
                    }
                    beta = min_(beta, best_val);
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
        return best_val;
    }
}

void Game::make_move_AI() {
    if (AI_level == 0) {
        int idx;
        // Easy AI, simply selects a random move
        while(true) {
            idx = rand() % alive_pieces.size();
            if (alive_pieces[idx]->side == "BLACK") {
                alive_pieces[idx]->mousePressEvent(nullptr);

                if (!alive_pieces[idx]->possible_moves.empty()) {
                    break;
                }
            }
        }
        auto &locations = alive_pieces[idx]->possible_moves;
        idx = rand() % alive_pieces[idx]->possible_moves.size();
        locations[idx]->mousePressEvent(nullptr);
    } else {
        Tile *best_move = nullptr;
        Piece *best_piece = nullptr;
        minmax(0, AI_level, "BLACK", -100000, +100000, &best_piece, &best_move);
        best_piece->mousePressEvent(nullptr);
        best_move->mousePressEvent(nullptr);
    }
}

void Game::start() {
    for(size_t i =0, n = listG.size(); i < n; i++) {
        remove_from_scene(listG[i]);
    }
    add_to_scene(turnDisplay);

    // Setup graveyards
    // White
    QGraphicsTextItem* whitePiece = new QGraphicsTextItem();
    whitePiece->setZValue(1);
    whitePiece->setDefaultTextColor(Qt::white);
    whitePiece->setFont(QFont("Helvetica" , 15));
    whitePiece->setPlainText("White Graveyard");
    whitePiece->setPos((TILE_WIDTH*2 - whitePiece->boundingRect().width())/2,
                       (TILE_WIDTH-whitePiece->boundingRect().height())/2);
    add_to_scene(whitePiece);
    QGraphicsTextItem *blackPiece = new QGraphicsTextItem();
    // Black
    blackPiece->setZValue(1);
    blackPiece->setDefaultTextColor(Qt::black);
    blackPiece->setFont(QFont("Helvetica" , 15));
    blackPiece->setPlainText("Black Graveyard");
    blackPiece->setPos(TILE_WIDTH*10 + (TILE_WIDTH*2 - blackPiece->boundingRect().width())/2,
                       (TILE_WIDTH-blackPiece->boundingRect().height())/2);
    add_to_scene(blackPiece);
    add_to_scene(check_display);
    chess->add_piece();
}

void Game::playAIeasy() {
    AI_level = 0;
    start();
}

void Game::playAImedium() {
    AI_level = 1;
    start();
}

void Game::playAIhard() {
    AI_level = 2;
    start();
}

void Game::startAI() {
    for(size_t i = 2, n = listG.size(); i < n; i++) {
        remove_from_scene(listG[i]);
    }
    vsAI = true;
    //create Easy Button
    Button * easyButton = new Button("Easy", TILE_WIDTH*5,TILE_WIDTH*4);
    connect(easyButton,SIGNAL(clicked()) , this , SLOT(playAIeasy()));
    add_to_scene(easyButton);
    listG.append(easyButton);
    // create Medium Button
    Button * mediumButton = new Button("Medium", TILE_WIDTH*5,TILE_WIDTH*5);
    connect(mediumButton, SIGNAL(clicked()),this,SLOT(playAImedium()));
    add_to_scene(mediumButton);
    draw_board();
    listG.append(mediumButton);
    //Create Hard Button
    Button * hardButton = new Button("Hard", TILE_WIDTH*5,TILE_WIDTH*6);
    connect(hardButton, SIGNAL(clicked()),this,SLOT(playAIhard()));
    add_to_scene(hardButton);
    draw_board();
    listG.append(hardButton);
}

void Game::draw_cemetary(int x, int y,QColor color) {
    deadHolder = new QGraphicsRectItem(x,y,TILE_WIDTH*2,TILE_WIDTH*10);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    deadHolder->setBrush(brush);
    add_to_scene(deadHolder);
}

void Game::draw_menu() {
    // Create the title
    QGraphicsTextItem *titleText = new QGraphicsTextItem("Qt Chess");
    titleText->setFont(QFont("Helvetica" , 40));
    titleText->setPos(TILE_WIDTH*3 + (TILE_WIDTH*6 - titleText->boundingRect().width())/2,
                      TILE_WIDTH*3 + (TILE_WIDTH - titleText->boundingRect().height())/2);
    titleText->setZValue(1);
    add_to_scene(titleText);
    listG.append(titleText);

    // Create Button box
    QGraphicsRectItem *box = new QGraphicsRectItem();
    box->setRect(3*TILE_WIDTH, 3*TILE_WIDTH, TILE_WIDTH*6, TILE_WIDTH*4);
    box->setBrush(QColor(87, 84, 82));
    box->setZValue(0);
    add_to_scene(box);
    listG.append(box);

    //create Play Button
    Button * playButton = new Button("Vs Player", TILE_WIDTH*5,TILE_WIDTH*4);
    connect(playButton,SIGNAL(clicked()) , this , SLOT(start()));
    add_to_scene(playButton);
    listG.append(playButton);

    //Create vs AI button
    Button * AIButton = new Button("Vs AI", TILE_WIDTH*5,TILE_WIDTH*5);
    connect(AIButton, SIGNAL(clicked()),this,SLOT(startAI()));
    add_to_scene(AIButton);
    draw_board();
    listG.append(AIButton);

    //Create Quit Button
    Button * quitButton = new Button("Quit", TILE_WIDTH*5,TILE_WIDTH*6);
    connect(quitButton, SIGNAL(clicked()),this,SLOT(close()));
    add_to_scene(quitButton);
    draw_board();
    listG.append(quitButton);
}

bool Game::check_diagonal(Piece *king) {
    QVector<QPair<int, int>> changes = {{1, 1}, {1, -1},
                                        {-1, 1}, {-1, -1}};
    for (size_t i = 0; i < 4; ++i) {
        const int change_row = changes[i].first;
        const int change_col = changes[i].second;
        int limit_row = change_row > 0 ? 8:-1;
        int limit_col = change_col > 0 ? 8:-1;
        for (int r = king->curr_tile->row + change_row, c = king->curr_tile->col + change_col;
             r != limit_row && c != limit_col;
             r += change_row, c+= change_col) {
            if (game_board[r][c]->curr_piece != nullptr) {
                const int code_queen = king->side == "WHITE" ? b_queen:w_queen;
                const int code_bishop = king->side == "WHITE" ? b_bishop:w_bishop;
                if (game_board[r][c]->curr_piece->code == code_queen ||
                    game_board[r][c]->curr_piece->code == code_bishop) {
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

bool Game::check_lines(Piece *king) {
    QVector<QPair<int, int>> changes = {{1, 0}, {-1, 0},
                                        {0, 1}, {0, -1}};
    for (size_t i = 0; i < 4; ++i) {
        const int change_row = changes[i].first;
        const int change_col = changes[i].second;
        int limit_row = change_row > 0 ? 8:-1;
        int limit_col = change_col > 0 ? 8:-1;
        for (int r = king->curr_tile->row + change_row, c = king->curr_tile->col + change_col;
             r != limit_row && c != limit_col;
             r += change_row, c+= change_col) {
            if (game_board[r][c]->curr_piece != nullptr) {
                const int code_queen = king->side == "WHITE" ? b_queen:w_queen;
                const int code_bishop = king->side == "WHITE" ? b_rook:w_rook;
                if (game_board[r][c]->curr_piece->code == code_queen ||
                    game_board[r][c]->curr_piece->code == code_bishop) {
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

bool Game::check_knight(Piece *king) {
    QVector<QPair<int, int>> changes = {{1, 2}, {1, -2},
                                        {-1, 2}, {-1, -2},
                                        {2, 1}, {2, -1},
                                        {-2, 1}, {-2, -1}};
    for (size_t i = 0; i < 8; ++i) {
        const int r = king->curr_tile->row + changes[i].first;
        const int c = king->curr_tile->col + changes[i].second;
        const int code_knight = king->side == "WHITE" ? b_knight:w_knight;
        if (r >= 0 && r <= 7 && c >= 0 && c <= 7 &&
            game_board[r][c]->curr_piece != nullptr && game_board[r][c]->curr_piece->code == code_knight) {
            return true;
        }
    }
    return false;
}

bool Game::check_pawn(Piece *king) {
    const int change_r = king->side == "WHITE" ? -1:1;
    const int code_pawn = king->side == "WHITE" ? b_pawn:w_pawn;
    int r = king->curr_tile->row + change_r;
    int c = king->curr_tile->col + 1;
    if (r >= 0 && r <= 7 && c >= 0 && c <= 7 &&
        game_board[r][c]->curr_piece != nullptr && game_board[r][c]->curr_piece->code == code_pawn) {
        return true;
    }
    c -= 2;
    if (r >= 0 && r <= 7 && c >= 0 && c <= 7 &&
        game_board[r][c]->curr_piece != nullptr && game_board[r][c]->curr_piece->code == code_pawn) {
        return true;
    }
    return false;
}

bool Game::check_king(Piece *king) {
    QVector<QPair<int, int>> changes = {{-1, -1}, {-1, 0}, {-1, 1},
                                        {0, -1}, {0, 1},
                                        {1, -1}, {1, 0}, {1, 1}};
    for (size_t i = 0; i < 8; ++i) {
        const int r = king->curr_tile->row + changes[i].first;
        const int c = king->curr_tile->col + changes[i].second;
        const int code_king = king->side == "WHITE" ? b_king:w_king;
        if (r >= 0 && r <= 7 && c >= 0 && c <= 7 &&
            game_board[r][c]->curr_piece != nullptr && game_board[r][c]->curr_piece->code == code_king) {
            return true;
        }
    }
    return false;
}

bool Game::is_in_check(QString side) {
    Piece *king = side == "WHITE" ? chess->king_w:chess->king_b;
    if (check_lines(king) || check_diagonal(king) || check_pawn(king) || check_knight(king) || check_king(king)) {
        return true;
    }
    return false;
}

bool Game::is_check_mate(QString side) {
    for (int i = 0; i < alive_pieces.size(); ++i) {
        if (alive_pieces[i]->side == side) {
            alive_pieces[i]->get_valid_moves();
            if(!alive_pieces[i]->possible_moves.empty()) {
                alive_pieces[i]->reset_tile_color();
                alive_pieces[i]->possible_moves.clear();
                return false;
            }
        }
    }
    return true;
}

void Game::reset() {
    for(size_t i =0, n = listG.size(); i < n; i++) {
        remove_from_scene(listG[i]);
    }
    turn  = "WHITE";
    turnDisplay->setPlainText(turn + " TURN");
    turnDisplay->setDefaultTextColor(Qt::white);
    check_display->setVisible(false);
    alive_pieces.clear();
    chess->reset();
}

void Game::game_over() {
    //Create game over text
    QGraphicsTextItem *game_over_text = new QGraphicsTextItem("Game Over");
    game_over_text->setFont(QFont("Helvetica" , 40));
    game_over_text->setPos(TILE_WIDTH*3 + (TILE_WIDTH*6 - game_over_text->boundingRect().width())/2,
                           TILE_WIDTH*3 + (TILE_WIDTH - game_over_text->boundingRect().height())/2);
    game_over_text->setZValue(1);
    add_to_scene(game_over_text);
    listG.append(game_over_text);

    // Create victory text
    QString winner = turn == "WHITE" ? "Black":"White";
    QGraphicsTextItem *victory_text = new QGraphicsTextItem(winner + " Victory!");
    victory_text->setFont(QFont("Helvetica" , 40));
    victory_text->setPos(TILE_WIDTH*3 + (TILE_WIDTH*6 - victory_text->boundingRect().width())/2,
                           TILE_WIDTH*4 + (TILE_WIDTH - victory_text->boundingRect().height())/2);
    victory_text->setZValue(1);
    victory_text->setDefaultTextColor(turn == "WHITE" ? Qt::black : Qt::white);
    add_to_scene(victory_text);
    listG.append(victory_text);

    // Create Button box
    QGraphicsRectItem *box = new QGraphicsRectItem();
    box->setRect(3*TILE_WIDTH, 3*TILE_WIDTH, TILE_WIDTH*6, TILE_WIDTH*4);
    box->setBrush(QColor(87, 84, 82));
    box->setZValue(0);
    add_to_scene(box);
    listG.append(box);

    //create Play again Button
    Button * playButton = new Button("Play Again", TILE_WIDTH*5,TILE_WIDTH*5);
    //playButton->setPos(TILE_WIDTH*5,TILE_WIDTH*5);
    connect(playButton,SIGNAL(clicked()) , this , SLOT(reset()));
    add_to_scene(playButton);
    listG.append(playButton);

    //Create Quit Button
    Button * quitButton = new Button("Quit", TILE_WIDTH*5,TILE_WIDTH*6);
    //quitButton->setPos(TILE_WIDTH*5,TILE_WIDTH*6);
    connect(quitButton, SIGNAL(clicked()),this,SLOT(close()));
    add_to_scene(quitButton);
    listG.append(quitButton);
}
