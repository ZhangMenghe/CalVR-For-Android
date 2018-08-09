#include "SpatialViz.h"
using namespace cvr;

bool SpatialViz::init() {
    _mainMenu = new SubMenu("SpatialViz", "SpatialViz");
    _mainMenu->setCallback(this);
    MenuSystem::instance()->addMenuItem(_mainMenu);

    _tetrisPuzzle = new MenuButton("Tetris Matching");
    _tetrisPuzzle->setCallback(this);
    _mainMenu->addItem(_tetrisPuzzle);

    return true;
}