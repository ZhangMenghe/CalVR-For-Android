#include "SpatialViz.h"
using namespace cvr;

bool SpatialViz::init() {
    // --------------- create the menu ---------------
    _mainMenu = new SubMenu("SpatialViz", "SpatialViz");
    _mainMenu->setCallback(this);
    MenuSystem::instance()->addMenuItem(_mainMenu);

    _mazePuzzleButton = new MenuButton("Maze Puzzle");
    _mazePuzzleButton->setCallback(this);
    _mainMenu->addItem(_mazePuzzleButton);

    _5x5puzzleButton = new MenuButton("5x5 Puzzle");
    _5x5puzzleButton->setCallback(this);
    _mainMenu->addItem(_5x5puzzleButton);

    _labyrinthPuzzle = new MenuButton("Labyrinth Puzzle");
    _labyrinthPuzzle->setCallback(this);
    _mainMenu->addItem(_labyrinthPuzzle);

    _tetrisPuzzle = new MenuButton("Tetris Matching");
    _tetrisPuzzle->setCallback(this);
    _mainMenu->addItem(_tetrisPuzzle);

    _tetrisPuzzle2 = new MenuButton("Tetris 2 Puzzle");
    _tetrisPuzzle2->setCallback(this);
    _mainMenu->addItem(_tetrisPuzzle2);

    _removePuzzles = new MenuButton("Remove Puzzles");
    _removePuzzles->setCallback(this);
    _mainMenu->addItem(_removePuzzles);

    _restartPhysics = new MenuButton("Restart Physics");
    _restartPhysics->setCallback(this);
    _mainMenu->addItem(_restartPhysics);

    return true;
}

void SpatialViz::menuCallback(cvr::MenuItem *item) {

}