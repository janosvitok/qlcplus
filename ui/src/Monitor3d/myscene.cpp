#include "myscene.h"

MyScene::MyScene()
{
    _root = new osg::Group;

    //floor creation
    float radius = 20.0f;
    osg::ref_ptr<osg::Geode> floorGeode = new osg::Geode();

    osg::ref_ptr<osg::Geometry> floorGeometry = new osg::Geometry();
    floorGeode->addDrawable( floorGeometry );

    osg::ref_ptr<osg::Vec3Array> floorVertices = new osg::Vec3Array();
    floorVertices->push_back( osg::Vec3( -1.0f * radius, -1.0f * radius, 0.0f) );
    floorVertices->push_back( osg::Vec3( -1.0f * radius,  1.0f * radius, 0.0f) );
    floorVertices->push_back( osg::Vec3(  1.0f * radius,  1.0f * radius, 0.0f) );
    floorVertices->push_back( osg::Vec3(  1.0f * radius, -1.0f * radius, 0.0f) );

    floorGeometry->setVertexArray( floorVertices );

    osg::ref_ptr<osg::DrawElementsUInt> floorFace =
            new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS , 0);
    for(unsigned int i = 0; i < 4; ++i ){
        floorFace->push_back( i );
    }
    floorGeometry->addPrimitiveSet( floorFace );

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    for (int i = 0; i < 4; ++i)
    {
    colors->push_back(osg::Vec4(0.54f, 0.27f, 0.07f, 1.0f) ); //brown
    }
    floorGeometry->setColorArray(colors);
    floorGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    _root->addChild( floorGeode );
    //end of floor creation

//    stageGeode stage = stageGeode();
//    _root->addChild( stage.get() );

//    int numberOfFixtures = 15;
//    float space = stage.getWidth() / (float)(numberOfFixtures + 1);
//    float posX = stage.getWidth() * -0.5f;
//    for( int i = 0; i < numberOfFixtures; ++i){
//    _cone.append(Cone());
////    _root->addChild( _cone.getGeode() );
//    //cone.changeColor(osg::Vec3(0.4f, 0.8f, 0.2f));
//        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
//        posX += space;
//        trans->setMatrix( osg::Matrix::rotate(osg::PI / 4, osg::Vec3d(-1, 0,  0)) * osg::Matrix::translate( osg::Vec3(posX, 8.0f, 7.0f)) );
//        trans->addChild( _cone.last().getGeode() );
//        _root->addChild( trans );
//        _cone.last().changeColor(osg::Vec3((float)(i+ 1)/(float)numberOfFixtures, 1.0f, 0.0f));
//        _cone.last().changeOpacity(0.6f);
//    }


}
