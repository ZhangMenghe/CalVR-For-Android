#include "SpatialViz.h"
using namespace cvr;

bool SpatialViz::init() {
    testItem1 = new MenuCheckbox("menuItem 1", false);
    testItem1->setCallback(this);
    MenuSystem::instance()->addMenuItem(testItem1);

    testItem2 = new MenuCheckbox("menuItem 2", false);
    testItem2->setCallback(this);
    MenuSystem::instance()->addMenuItem(testItem2);

    testItem3 = new MenuCheckbox("menuItem 3", false);
    testItem3->setCallback(this);
    MenuSystem::instance()->addMenuItem(testItem3);
    return true;
}

void SpatialViz::menuCallback(cvr::MenuItem *item) {

}