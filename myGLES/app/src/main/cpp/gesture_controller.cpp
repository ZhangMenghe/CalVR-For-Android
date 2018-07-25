//
// Created by menghe on 7/22/2018.
//

#include <osg/ShapeDrawable>

#include "gesture_controller.h"
namespace {
    // Fragment shader to display everything in red colour.
    static const char shaderFragment[] =
            "void main() {                             \n"
                    "  gl_FragColor = vec4(0.5, 0.3, 0.3, 1.0);\n"
                    "}                                         \n";
// Geometry shader to pass geometry vertices to fragment shader.
    static const char shaderVertex[] =
            "void main() {                                              \n"
                    "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
                    "}                                                          \n";

}
void gesture_controller::createNode(osg_controller::osgController * osgApp, AAssetManager * manager) {
    LOGE("================called from create interaction node================");
    shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(osg::Vec3(.0f,.0f,.0f), 0.3f));

    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    osg::Program * prog = osg_utils::createShaderProgram(shaderVertex, shaderFragment);
    shape->getOrCreateStateSet()->setAttribute(prog);
    node->addDrawable(shape.get());
    osgApp->addNode(node.get());
    view = osgApp->getViewer();
}
