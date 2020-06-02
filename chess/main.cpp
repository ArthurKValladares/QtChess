#include <QApplication>
#include "game.h"

// TODO:
// Test upgrade for AI and vs Player
Game *game;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    game = new Game();
    game->show();
    game->draw_menu();
    return a.exec();
}
