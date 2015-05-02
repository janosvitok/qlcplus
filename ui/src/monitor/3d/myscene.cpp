#include "myscene.h"
#include <osgDB/ReadFile>
#include "qlcfile.h"
#include "qlcconfig.h"

#include <osgShadow/ShadowMap>

class FixturesCallback : public osg::NodeCallback
{
public:
   FixturesCallback( OsgScene * scene )
    : _scene(scene) {}

   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv){
        _scene->setFixturesChanges();
        traverse(node, nv);
    }

private:
    OsgScene * _scene;
};

OsgScene::OsgScene()
    : _changed(false)
{
    _root = new osgShadow::ShadowedScene;

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

////end of floor creation

//////stage creation
//    osg::ref_ptr<osg::Geode> stage = new osg::Geode();
    osg::Node* stage = osgDB::readNodeFile((QLCFile::systemDirectory(MODELSDIR).path()
                                 +QDir::separator()+"stage.osgt").toLatin1().constData());

    _stageWidth = stage->getBound().radius();
    //////end of stage creation

    _root->addChild( floorGeode );
    _root->addChild( stage );

    //initializing shadows, lights maybe

    const int ReceivesShadowTraversalMask = 0x1;
    const int CastsShadowTraversalMask = 0x2;


    _root->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
    _root->setCastsShadowTraversalMask(CastsShadowTraversalMask);

    osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
    _root->setShadowTechnique(sm.get());

    int mapres = 1024;
    sm->setTextureSize(osg::Vec2s(mapres,mapres));

    stage->setNodeMask(CastsShadowTraversalMask);
    floorGeode->setNodeMask(ReceivesShadowTraversalMask);

    //skuska svetla
    osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
    ls->getLight()->setPosition(osg::Vec4(0, 8.0f, 7.0f, 0));
    osg::Vec3 lightdir = osg::Vec3(0, -8.0f, -7.0f);
    lightdir.normalize();
    ls->getLight()->setDirection(lightdir);
    ls->getLight()->setSpotCutoff(25.0f);
    _root->addChild(ls.get());
    sm->setLight(ls.get());
    //koniec skusky svetla

    _root->setUpdateCallback(new FixturesCallback(this) );
}

OsgScene::~OsgScene()
{
    foreach(Fixture3d * fixture, m_fixtures)
    {
        delete fixture;
    }
    m_fixtures.clear();

    foreach(Object3d * obj, m_objects)
    {
        delete obj;
    }
    m_objects.clear();
}

void OsgScene::update(const QByteArray &ua)
{
    QMutexLocker locker(&_mutex);
    _ua = ua;
    _changed = true;
}

void OsgScene::setToBeMovable(osg::Drawable *shape)
{
    foreach(Fixture3d * fixture, m_fixtures)
    {
        if (fixture->contains(shape))
        {
            fixture->setDraggerGVisibility(true);
        }
        else
        {
            fixture->setDraggerGVisibility(false);
        }
        fixture->setDraggerRVisibility(false);
    }
    foreach(Object3d * obj, m_objects)
    {
        if (obj->contains(shape))
        {
            obj->setDraggerGVisibility(true);
        }
        else
        {
            obj->setDraggerGVisibility(false);
        }
        obj->setDraggerRVisibility(false);
    }
}

void OsgScene::setToBeRotatable(osg::Drawable *shape)
{
    foreach(Fixture3d * fixture, m_fixtures)
    {
        if (fixture->contains(shape))
        {
            fixture->setDraggerRVisibility(true);
        }
        else
        {
            fixture->setDraggerRVisibility(false);
        }
        fixture->setDraggerGVisibility(false);
    }
    foreach(Object3d * obj, m_objects)
    {
        if (obj->contains(shape))
        {
            obj->setDraggerRVisibility(true);
        }
        else
        {
            obj->setDraggerRVisibility(false);
        }
        obj->setDraggerGVisibility(false);
    }
}

void OsgScene::addFixture(Doc * doc, quint32 fid)
{
    Fixture3d * fixture = new Fixture3d(doc, fid);
    m_fixtures[fid] = fixture;
    _root->addChild( fixture->getFixture() );
}

void OsgScene::addObject(QString const & modelPath)
{
    Object3d * obj = new Object3d(modelPath);
    m_objects.push_back(obj);
    _root->addChild(obj->node());
}

void OsgScene::removeFixture(quint32 fid)
{
    Fixture3d * fixture = m_fixtures[fid];
    if (fixture == NULL)
        return;
    _root->removeChild(fixture->getFixture());
    m_fixtures.remove(fid);
}

void OsgScene::setFixturesChanges()
{
    QByteArray ua;

    {
        QMutexLocker locker(&_mutex);
        if (!_changed)
            return;
        ua.swap(_ua);
        _changed = false;
    }

    foreach(Fixture3d * fixture, m_fixtures)
    {
        fixture->updateValues(ua);
    }
}
