#include "fixture3d.h"
#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include "qlcfile.h"
#include "qlcconfig.h"

Fixture3d::Fixture3d(Doc * doc, quint32 fid)
    : MonitorFixtureBase(doc, fid)
    , _invisibleKolor(0.0f)
{
    initialize();
//    _fixture = new osg::Group();

    createParCan();
}
MonitorFixtureHead * Fixture3d::createHead(Fixture & fixture, int head)
{
    return new MonitorFixtureHead(fixture, head);
}

void Fixture3d::createParCan()
{
    osg::Node * head = osgDB::readNodeFile((QLCFile::systemDirectory(MODELSDIR).path() + QDir::separator() + "PAR64.osgt").toLatin1().constData());
//   _fixture->addChild(head);

    _lightConeGeode = new osg::Geode();
    osg::ref_ptr<osg::Geometry> pyramidGeometry = new osg::Geometry();
    _lightConeGeode->addDrawable(pyramidGeometry);
    pyramidGeometry->setDataVariance( osg::Object::DYNAMIC );
    pyramidGeometry->setUseDisplayList( false );
//    _fixture->addChild(_lightConeGeode);

    osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array();
    pyramidVertices->push_back( osg::Vec3(  0,  0, 0) ); // peak
    int faces = 16;
    float width = 1.26492; //http://www.fas.harvard.edu/~loebinfo/loebinfo/lighting/lighting.html#PAR MFL transformed to metrics
    float height =  1.26492;
    float lenght = 6.096;
    double partOfCircle = osg::PI * 2.0 / (double)faces ;
    double position = 0.0;
    for (int i = 0; i < faces; ++i){
        pyramidVertices->push_back( osg::Vec3( sin(position) * width, cos(position) * height, -(lenght)) ); // points at base
        position += partOfCircle;
    }
    pyramidGeometry->setVertexArray( pyramidVertices );

    //cone faces creation
    osg::ref_ptr<osg::DrawElementsUInt> pyramidFace =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_FAN, 0);
    pyramidFace->push_back(0);
    for(int i = 1; i <= faces; ++i){
        pyramidFace->push_back(i);
    }
    pyramidFace->push_back(1);
    pyramidGeometry->addPrimitiveSet(pyramidFace);


    _colors = new osg::Vec4Array();
    _colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 0 white
    for (int i = 0; i < faces; ++i)
    {
    _colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f) ); //index i off
    }

    pyramidGeometry->setColorArray(_colors);
    pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::StateSet* stateset = _lightConeGeode->getOrCreateStateSet();
    stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    stateset->setAttributeAndModes( blendFunc );

    osg::CullFace* cull = new osg::CullFace();
    cull->setMode(osg::CullFace::FRONT);
    stateset->setAttributeAndModes(cull, osg::StateAttribute::ON);
    //all fixture transformations
    _transG = new osg::MatrixTransform;
    _transR = new osg::MatrixTransform;
    _transQLC = new osg::MatrixTransform;
    _transG->setMatrix(osg::Matrix::translate( osg::Vec3(0.0f, 8.0f, 7.0f) ) );
    _transR->setMatrix( osg::Matrix::rotate( osg::PI / 8, osg::Vec3d(-1, 0,  0) ) );
    _transQLC->addChild( head );
    _transQLC->addChild( _lightConeGeode );
    _transR->addChild( _transQLC );

/// translation by user with mouse
    _transR->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
    _transG->addChild( _transR );
    _draggerG = new osgManipulator::TranslateAxisDragger();
    _draggerG->setupDefaultGeometry();
    _transG->addChild(_draggerG);
    _draggerG->setHandleEvents(false);
//    _draggerG->setActivationKeyEvent('g');
    _draggerG->addTransformUpdating( _transR );
    _draggerG->setNodeMask(0);

///// rotation by user with mouse
    _transQLC->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
    _transR->addChild( _transQLC );
    _draggerR = new osgManipulator::TrackballDragger();
    _draggerR->setupDefaultGeometry();
    float scale = head->getBound().radius() * 1.6;
    _draggerR->setMatrix(osg::Matrix::scale(scale, scale, scale) *
                       osg::Matrix::translate(head->getBound().center()));
    _transR->addChild(_draggerR);
    _draggerR->setHandleEvents(false);
////    _draggerR->setActivationKeyEvent('r');
    _draggerR->addTransformUpdating( _transQLC );
    _draggerR->setNodeMask(0);
    osg::StateSet* draggerR_stateset = _draggerR->getOrCreateStateSet();
    draggerR_stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

}

void Fixture3d::updateValues(QByteArray const & ua)
{
    foreach(MonitorFixtureHead *head, m_heads)
    {
        QColor col = head->computeColor(ua);
        changeColor(osg::Vec3(col.redF(), col.greenF(), col.blueF()), true);
        
        changeOpacity(head->computeAlpha(ua)/255.0, true);

        //qreal const panPosition = head->computePanPosition(ua);
        //qreal const tiltPosition = head->computeTiltPosition(ua);
        //
        //if (head->m_panDegrees != panPosition || head->m_tiltDegrees != tiltPosition)
        //{
        //    head->m_panDegrees = panPosition;
        //    head->m_tiltDegrees = tiltPosition;
        //    needUpdate = true;
        //}
    }
}

void Fixture3d::changeColor(osg::Vec3 const & colorValue, bool overwrite)
{
    if(_colors){
        float maxValue = ((colorValue.x() > colorValue.y()) ? colorValue.x() : colorValue.y());
        maxValue = ((maxValue > colorValue.z()) ? maxValue : colorValue.z());
        _invisibleKolor = 1.0 - maxValue;
        for(unsigned int i = 0; i < _colors->size(); ++i){
            osg::Vec4 *color = &_colors->operator [](i);
            if(overwrite){
                color->x() = colorValue.x() + _invisibleKolor;
                color->y() = colorValue.y() + _invisibleKolor;
                color->z() = colorValue.z() + _invisibleKolor;
            }
            else{
                color->x() += colorValue.x();
                color->y() += colorValue.y();
                color->z() += colorValue.z();
            }
        }
    }
}

void Fixture3d::changeOpacity(float opacityValue, bool overwrite)
{
    osg::Vec4 *color = &_colors->operator [](0);

    if (overwrite){
        color->w() = opacityValue - _invisibleKolor ;
    }
    else{
        color->w() = opacityValue - _invisibleKolor + _alpha;
    }
}

void Fixture3d::moveHead(float posX, float posY, float posZ)
{
    _transG->setMatrix( osg::Matrix::translate( osg::Vec3(posX, posY, posZ)) );
}

void Fixture3d::setDraggerGVisibility(bool visible)
{
    if(_visibleG == visible){ return; }
    _visibleG = visible;
    if(_visibleG){
        _draggerG->setNodeMask(~0);
        setDraggerRVisibility(false);
    }
    else{
        _draggerG->setNodeMask(0);
    }
    _draggerG->setHandleEvents(_visibleG);
}

void Fixture3d::setDraggerRVisibility(bool visible)
{
    if(_visibleR == visible){ return; }
    _visibleR = visible;
    if(_visibleR){
        _draggerR->setNodeMask(~0);
        setDraggerGVisibility(false);
    }
    else{
        _draggerR->setNodeMask(0);
    }
    _draggerR->setHandleEvents(_visibleR);
}
