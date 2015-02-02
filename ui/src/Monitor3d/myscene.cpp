#include "myscene.h"

class FixturesCallback : public osg::NodeCallback
{
public:
   FixturesCallback( MyScene * scene )
    : _scene(scene) {}

   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv){
        _scene->urobcomas();
        traverse(node, nv);
    }

private:
    MyScene * _scene;
};

MyScene::MyScene()
    : _changed(false)
{
    _root = new osg::Group;

////floor creation
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
////end of floor creation

////stage creation
    osg::ref_ptr<osg::Geode> stage = new osg::Geode();
    _stageWidth = 30.0;
    _stageDepth = 20.0;
    _stageHeight = 1.0;

    osg::ref_ptr<osg::Geometry> stageGeometry = new osg::Geometry();
    stage->addDrawable( stageGeometry );

    osg::ref_ptr<osg::Vec3Array> stageVertices = new osg::Vec3Array();
    stageVertices->push_back( osg::Vec3( -0.5 * _stageWidth, 0, 0 ) );
    stageVertices->push_back( osg::Vec3(  0.5 * _stageWidth, 0, 0 ) );
    stageVertices->push_back( osg::Vec3(  0.5 * _stageWidth, _stageDepth, 0 ) );
    stageVertices->push_back( osg::Vec3( -0.5 * _stageWidth, _stageDepth, 0 ) );
    stageVertices->push_back( osg::Vec3( -0.5 * _stageWidth, 0, _stageHeight ) );
    stageVertices->push_back( osg::Vec3(  0.5 * _stageWidth, 0, _stageHeight ) );
    stageVertices->push_back( osg::Vec3(  0.5 * _stageWidth, _stageDepth, _stageHeight ) );
    stageVertices->push_back( osg::Vec3( -0.5 * _stageWidth, _stageDepth, _stageHeight ) );
    stageGeometry->setVertexArray( stageVertices );

    osg::ref_ptr<osg::DrawElementsUInt> frontFace =
            new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS);
    frontFace->push_back( 0 );
    frontFace->push_back( 1 );
    frontFace->push_back( 5 );
    frontFace->push_back( 4 );
    stageGeometry->addPrimitiveSet( frontFace );

    osg::ref_ptr<osg::DrawElementsUInt> rightFace =
            new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS);
    rightFace->push_back( 1 );
    rightFace->push_back( 2 );
    rightFace->push_back( 6 );
    rightFace->push_back( 5 );
    stageGeometry->addPrimitiveSet( rightFace );

    osg::ref_ptr<osg::DrawElementsUInt> leftFace =
            new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS);
    leftFace->push_back( 0 );
    leftFace->push_back( 4 );
    leftFace->push_back( 7 );
    leftFace->push_back( 3 );
    stageGeometry->addPrimitiveSet( leftFace );

    osg::ref_ptr<osg::DrawElementsUInt> backFace =
            new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS);
    backFace->push_back( 2 );
    backFace->push_back( 3 );
    backFace->push_back( 7 );
    backFace->push_back( 6 );
    stageGeometry->addPrimitiveSet( backFace );

    osg::ref_ptr<osg::DrawElementsUInt> topFace =
            new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS);
    topFace->push_back( 4 );
    topFace->push_back( 5 );
    topFace->push_back( 6 );
    topFace->push_back( 7 );
    stageGeometry->addPrimitiveSet( topFace );

    _root->addChild( stage );
////end of stage creation

    int numberOfFixtures = 15;
    float space = _stageWidth / (float)(numberOfFixtures + 1);
    float posX = _stageWidth * -0.5f;
    for( int i = 0; i < numberOfFixtures; ++i){
    _fixture3d.append(Fixture3d());
        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
        posX += space;
        trans->setMatrix( osg::Matrix::rotate(osg::PI / 4, osg::Vec3d(-1, 0,  0)) * osg::Matrix::translate( osg::Vec3(posX, 20.0f, 7.0f)) );
        trans->addChild( _fixture3d.last().getFixture() );
        _root->addChild( trans );
        _fixture3d.last().changeColor(osg::Vec3((float)(i+ 1)/(float)numberOfFixtures, 1.0f, 0.0f));
        _fixture3d.last().changeOpacity(0.6f);
    }

    _root->setUpdateCallback(new FixturesCallback(this) );
}

void MyScene::update(const QByteArray &ua)
{
    QMutexLocker locker(&_mutex);
    _ua = ua;
    _changed = true;
}

void MyScene::urobcomas()
{
    QByteArray ua;

    {
        QMutexLocker locker(&_mutex);
        if (!_changed)
            return;
        ua.swap(_ua);
        _changed = false;
    }

    for( int i=0; i < _fixture3d.count(); ++i)
    {
        uchar value = 0;
        if (ua.size() > i )
            value = (uchar)ua.at(i);

        _fixture3d[i].changeOpacity( (float)value/255.0f );
    }
}
