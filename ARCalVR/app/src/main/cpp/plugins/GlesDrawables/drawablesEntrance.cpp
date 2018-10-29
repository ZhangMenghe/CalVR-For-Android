#include <cvrUtil/AndroidStdio.h>
#include <osg/LineWidth>
#include <osgUtil/Tessellator>
#include <osg/Texture>
#include "drawablesEntrance.h"
#include "planeDrawable.h"
#include <osg/Texture2D>
#include <cvrUtil/AndroidHelper.h>
#include <osg/ShapeDrawable>

using namespace osg;
using namespace cvr;
void GlesDrawables:: tackleHitted(osgUtil::LineSegmentIntersector::Intersection result ){
    for(auto itr = result.drawable->getParents().begin(); itr!=result.drawable->getParents().end(); itr++){
       auto got = _obj_color_map.find((*itr)->getName());
        LOGE("====%s", (*itr)->getName().c_str());
        if(got!=_obj_color_map.end()){
            IsectInfo isect;
            isect.point = result.getWorldIntersectPoint();
            isect.normal = result.getWorldIntersectNormal();
            isect.geode = dynamic_cast<Geode*>(*itr);
            TrackingManager::instance()->setIntersectPoint(isect.point);
            _obj_color_map[isect.geode->getName()]->set(Vec4f(.0f, 1.0f, .0f, 1.0f));

            return;
        }
    }
}
void GlesDrawables::initMenuButtons() {
    _pointButton = new MenuButton("Show PointCloud");
    _pointButton->setCallback(this);
    _mainMenu->addItem(_pointButton);

    _planeButton = new MenuButton("Show Detected Planes");
    _planeButton->setCallback(this);
    _mainMenu->addItem(_planeButton);

    _strokeButton = new MenuButton("Show Stroke Ray");
    _strokeButton->setCallback(this);
    _mainMenu->addItem(_strokeButton);
}

bool GlesDrawables::init() {
    // --------------- create the menu ---------------
    _mainMenu = new SubMenu("GlesDrawable", "GlesDrawable");
    _mainMenu->setCallback(this);
    MenuSystem::instance()->addMenuItem(_mainMenu);
    initMenuButtons();

    _root = new Group;
    _objects = new Group;
    _root->addChild(_objects);

    SceneManager::instance()->getSceneRoot()->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    //bool navigation, bool movable, bool clip, bool contextMenu, bool showBounds
    rootSO= new SceneObject("glesRoot", false, false, false, false, false);
    rootSO->addChild(_root);

    PluginHelper::registerSceneObject(rootSO, "GlesDrawablesPlugin");
    rootSO->dirtyBounds();
    rootSO->attachToScene();

    _strokeDrawable = new strokeDrawable;
    _root->addChild(_strokeDrawable->createDrawableNode());

    _pointcloudDrawable = new pointDrawable;
    _root->addChild(_pointcloudDrawable->createDrawableNode());

//    createConvexPolygon(_root, Vec3f(.0,.0,.0));
//    createObject(_objects, Vec3f(.0,.0,-0.1f)); // opengl coordinate
//    createDebugOSGSphere(_objects, Vec3f(.0,0.5f,.0f));

    return true;
}

void GlesDrawables::menuCallback(cvr::MenuItem *item) {
}
void GlesDrawables::preFrame() {
    //TODO:MAKE IT AVIALIABLE IN CALVR
    osg::Vec3 pointerStart, pointerEnd;
    pointerStart = TrackingManager::instance()->getHandMat(0).getTrans();
    pointerEnd.set(0.0f,10000.0f,0.0f);
    pointerEnd = pointerEnd * TrackingManager::instance()->getHandMat(0);
    //Add intersection detector

    osg::ref_ptr<osgUtil::LineSegmentIntersector> handseg = new osgUtil::LineSegmentIntersector(pointerStart, pointerEnd);
    osgUtil::IntersectionVisitor iv( handseg.get() );
    _objects->accept( iv );
    if ( handseg->containsIntersections()){
        for(auto itr=handseg->getIntersections().begin(); itr!=handseg->getIntersections().end(); itr++)
            tackleHitted(*itr);
    }
}
void GlesDrawables::postFrame() {
    _pointcloudDrawable->updateOnFrame();
    cvr::planeMap map = ARCoreManager::instance()->getPlaneMap();
    if(_plane_num < map.size()){
        for(int i= _plane_num; i<map.size();i++){
            planeDrawable * pd = new planeDrawable();
            _root->addChild(pd->createDrawableNode());
            _planeDrawables.push_back(pd);
        }
        _plane_num = map.size();
    }
    auto planeIt = map.begin();
    for(int i=0; i<_plane_num; i++,planeIt++)
        _planeDrawables[i]->updateOnFrame(planeIt->first, planeIt->second);
    Vec3f isPoint;Vec2f offset;
    if(TrackingManager::instance()->getIsPoint(isPoint)){
        TrackingManager::instance()->getTouchOffset(offset);
        _strokeDrawable->updateOnFrame(TrackingManager::instance()->getHandMat(0).getTrans(),
                                       isPoint, offset);
        _strokeDrawable->getGLNode()->setNodeMask(0xFFFFFF);
    } else
        _strokeDrawable->getGLNode()->setNodeMask(0x0);


    size_t anchor_num = ARCoreManager::instance()->getAnchorSize();
    if( anchor_num != 0){
        if(_objNum < anchor_num){
            for(int i=_objNum; i<anchor_num; i++){
                Matrixf modelMat;
                if(!ARCoreManager::instance()->getAnchorModelMatrixAt(modelMat, i))
                    break;
                createObject(_objects, modelMat);
            }

        }
        _objNum = anchor_num;
    }
}


void GlesDrawables::createObject(osg::Group *parent, Matrixf modelMat) {
    Transform objectTrans = new MatrixTransform;

    std::string fhead(getenv("CALVR_RESOURCE_DIR"));
    osg::ref_ptr<Node> objNode = osgDB::readNodeFile(fhead + "models/andy.obj");

    Program * program =assetLoader::instance()->createShaderProgramFromFile("shaders/object.vert","shaders/object.frag");
    osg::StateSet * stateSet = objNode->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    Uniform * baseColor = new osg::Uniform("uBaseColor", osg::Vec4f(1.0f, .0f, .0f, 1.0f));
    stateSet->addUniform(baseColor);

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess",
                                           64.0f) );
    stateSet->addUniform( new osg::Uniform("lightPosition",
                                           osg::Vec3(0,0,1)));
    stateSet->addUniform(new osg::Uniform("uScale", 0.1f));

    Uniform * viewUniform = new Uniform(Uniform::FLOAT_MAT4, "uView");
    viewUniform->setUpdateCallback(new viewMatrixCallback);
    stateSet->addUniform(viewUniform);

    Uniform * projUniform = new Uniform(Uniform::FLOAT_MAT4, "uProj");
    projUniform->setUpdateCallback(new projMatrixCallback);
    stateSet->addUniform(projUniform);

    //uModel
    Uniform * modelUniform = new Uniform(Uniform::FLOAT_MAT4, "uModel");
    modelUniform->set(modelMat);
    stateSet->addUniform(modelUniform);


    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage( osgDB::readImageFile(fhead+"textures/andy.png") );
    texture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
    texture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
    texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );


    stateSet->setTextureAttributeAndModes(0, texture.get(), osg::StateAttribute::ON);
    stateSet->addUniform(new osg::Uniform("uSampler", 0));

    objectTrans->addChild(objNode.get());
    parent->addChild(objectTrans.get());
    _obj_color_map[objNode->getName()] = baseColor;
}
void GlesDrawables::createConvexPolygon(osg::Group *parent, osg::Vec3f pos){
    // The vertex array shared by both the polygon and the border
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(8);
    (*vertices)[0].set( 0.0f, 0.0f, 0.0f );
    (*vertices)[1].set( 3.0f, 0.0f, 0.0f );
    (*vertices)[2].set( 3.0f, 0.0f, 3.0f );
    (*vertices)[3].set( 0.0f, 0.0f, 3.0f );
    (*vertices)[4].set( 1.0f, 0.0f, 1.0f );
    (*vertices)[5].set( 2.0f, 0.0f, 1.0f );
    (*vertices)[6].set( 2.0f, 0.0f, 2.0f );
    (*vertices)[7].set( 1.0f, 0.0f, 2.0f );

    // The normal array
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(1);
    (*normals)[0].set( 0.0f,-1.0f, 0.0f );

    // Construct the polygon geometry
    osg::ref_ptr<osg::Geometry> polygon = new osg::Geometry;
    polygon->setVertexArray( vertices.get() );
    polygon->setNormalArray( normals.get() );
    polygon->setNormalBinding( osg::Geometry::BIND_OVERALL );
    polygon->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );
    polygon->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 4, 4) );

    osgUtil::Tessellator tessellator;
    tessellator.setTessellationType( osgUtil::Tessellator::TESS_TYPE_GEOMETRY );
    tessellator.setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD );
    tessellator.retessellatePolygons( *polygon );

    // Construct the borderlines geometry
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
    (*colors)[0].set( 1.0f, 1.0f, 0.0f, 1.0f );

    osg::ref_ptr<osg::Geometry> border = new osg::Geometry;
    border->setVertexArray( vertices.get() );
    border->setColorArray( colors.get() );
    border->setColorBinding( osg::Geometry::BIND_OVERALL );
    border->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 0, 4) );
    border->addPrimitiveSet( new osg::DrawArrays(GL_LINE_LOOP, 4, 4) );
    border->getOrCreateStateSet()->setAttribute( new osg::LineWidth(5.0f) );

    // Add them to the scene graph
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( polygon.get() );
    geode->addDrawable( border.get() );

    ///////use shader
    Program * program =assetLoader::instance()->createShaderProgramFromFile("shaders/polygon.vert","shaders/polygon.frag");
    osg::StateSet * stateSet = geode->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    Uniform * mvpUniform = new Uniform(Uniform::FLOAT_MAT4, "uarMVP");
    mvpUniform->setUpdateCallback(new mvpCallback);
    stateSet->addUniform(mvpUniform);

    //uModel
    Uniform * modelUniform = new Uniform(Uniform::FLOAT_MAT4, "uModel");
    modelUniform->set(Matrixf::translate(pos));
    stateSet->addUniform(modelUniform);

    parent->addChild(geode);
}
void GlesDrawables::createDebugOSGSphere(osg::Group *parent,osg::Vec3 pos) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(pos, 0.05f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    Program * program = assetLoader::instance()->createShaderProgramFromFile("shaders/lighting.vert","shaders/lighting.frag");

    osg::StateSet * stateSet = shape->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess", 64.0f) );

    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0,0,1)));

    Uniform * baseColor = new osg::Uniform("uBaseColor", osg::Vec4f(1.0f, .0f, .0f, 1.0f));
    stateSet->addUniform(baseColor);

    node->addDrawable(shape.get());
    node->setName("miemie");
    _obj_color_map[node->getName()] = baseColor;
    parent->addChild(node);
}