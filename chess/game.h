#ifndef GAME_H
#define GAME_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include "board.h"
#include "piece.h"

#define TILE_WIDTH 100

class Game:public QGraphicsView
{
    Q_OBJECT
public:
    //Constructors
    Game(QWidget *parent = 0);

    // "Graphics" methods
    void draw_cemetary(int x, int y, QColor color);
    void draw_board();
    void draw_dead_pieces(bool is_white);
    void draw_menu();
    void swicth_turns();

    // Set-up/Clean-up methods
    void put_in_cemetary(Piece *piece);
    void add_to_scene(QGraphicsItem *item);
    void remove_from_scene(QGraphicsItem *item);

    // Game Logic
    bool check_diagonal(Piece *king);
    bool check_lines(Piece *king);
    bool check_knight(Piece *king);
    bool check_pawn(Piece *king);
    bool check_king(Piece *king);
    bool is_in_check(QString side);
    bool is_check_mate(QString side);
    void game_over();

    // AI methods/members
    bool vsAI = false;
    char AI_level = 0;
    int minmax(int depth, const int max_depth, QString side, int alpha, int beta, Piece **best_piece, Tile **best_pos);
    int piece_value(char piece, size_t r, size_t c);
    int evaluation_function();
    void make_move_AI();

    // public members
    Piece *moving_piece;
    Tile *game_board[8][8];
    QGraphicsTextItem *check_display;
    QList<Piece*> alive_pieces;
    QString turn;

public slots:
    void start();
    void reset();
    void startAI();
    void playAIeasy();
    void playAImedium();
    void playAIhard();
private:
    QGraphicsScene *gameScene;
    QList <Piece *> whiteDead;
    QList <Piece *> blackDead;
    QGraphicsRectItem * deadHolder;
    Board *chess;
    QList <QGraphicsItem *> listG;
    QGraphicsTextItem * turnDisplay;
};

#endif // GAME_H
