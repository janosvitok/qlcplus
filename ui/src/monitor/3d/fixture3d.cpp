#include "fixture3d.h"
#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include "qlcfile.h"
#include "qlcconfig.h"

#include <QDebug>

Fixture3d::Fixture3d(Doc * doc, quint32 fid)
    : MonitorFixtureBase(doc, fid)
    , m_visibleG(false)
    , m_visibleR(false)
    , m_invisibleKolor(0.0f)
    , m_alpha(0.0f)
{
    initialize();
    createFixture();
}

MonitorFixtureHead * Fixture3d::createHead(Fixture & fixture, int head)
{
    return new MonitorFixtureHead(fixture, head);
}

osg::Node * Fixture3d::createParCan()
{
    return osgDB::readNodeFile(
        (QLCFile::systemDirectory(MODELSDIR).path() + QDir::separator() + "PAR64.osgt").toLatin1().constData());
}

void Fixture3d::createLightBeam()
{
    m_lightConeGeode = new osg::Geode();
    osg::ref_ptr<osg::Geometry> pyramidGeometry = new osg::Geometry();
    m_lightConeGeode->addDrawable(pyramidGeometry);
    pyramidGeometry->setDataVariance(osg::Object::DYNAMIC);
    pyramidGeometry->setUseDisplayList(false);

    osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array();
    pyramidVertices->push_back(osg::Vec3(0, 0, 0)); // peak
    int faces = 16;
    float width = 1.26492; //http://www.fas.harvard.edu/~loebinfo/loebinfo/lighting/lighting.html#PAR MFL transformed to metrics
    float height = 1.26492;
    float lenght = 6.096;
    double partOfCircle = osg::PI * 2.0 / (double)faces ;
    double position = 0.0;
    for (int i = 0; i < faces; ++i)
    {
        pyramidVertices->push_back(osg::Vec3(sin(position) * width, cos(position) * height, -(lenght))); // points at base
        position += partOfCircle;
    }
    pyramidGeometry->setVertexArray(pyramidVertices);

    //cone faces creation
    osg::ref_ptr<osg::DrawElementsUInt> pyramidFace =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_FAN, 0);
    pyramidFace->push_back(0);
    for (int i = 1; i <= faces; ++i)
    {
        pyramidFace->push_back(i);
    }
    pyramidFace->push_back(1);
    pyramidGeometry->addPrimitiveSet(pyramidFace);

    m_colors = new osg::Vec4Array();
    m_colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 0 white
    for (int i = 0; i < faces; ++i)
    {
        m_colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f) ); //index i off
    }

    pyramidGeometry->setColorArray(m_colors);
    pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::StateSet* stateset = m_lightConeGeode->getOrCreateStateSet();
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateset->setAttributeAndModes(blendFunc);

    osg::CullFace* cull = new osg::CullFace();
    cull->setMode(osg::CullFace::FRONT);
    stateset->setAttributeAndModes(cull, osg::StateAttribute::ON);
}

void Fixture3d::createFixture()
{
    m_fixture = createParCan();
    createLightBeam();
    m_root = new osg::Group();

    //all fixture transformations
    m_transPan = new osg::MatrixTransform;
//    m_transPan->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    m_transTilt = new osg::MatrixTransform;
//    m_transTilt->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    m_transG = new osg::MatrixTransform;
//    m_transG->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 8.0f, 7.0f)));
//    m_trans->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    m_transR = new osg::MatrixTransform;
//    m_transR->setMatrix(osg::Matrix::rotate( osg::PI / 8, osg::Vec3d(-1, 0, 0)));;
//    m_transG->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    m_draggerG = new osgManipulator::TranslateAxisDragger();
    m_draggerG->setupDefaultGeometry();
    m_draggerG->setHandleEvents(false);
    m_draggerG->addTransformUpdating(m_transG);
    m_draggerG->setNodeMask(0);

    m_draggerR = new osgManipulator::TrackballDragger();
    m_draggerR->setupDefaultGeometry();
    float scale = m_fixture->getBound().radius() * 1.6;
    m_draggerR->setMatrix(osg::Matrix::scale(scale, scale, scale) *
                       osg::Matrix::translate(m_fixture->getBound().center()));
    m_draggerR->setHandleEvents(false);
    m_draggerR->addTransformUpdating(m_transR);
    m_draggerR->setNodeMask(0);
    m_draggerR->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    m_root->addChild(m_transG);
    m_root->addChild(m_draggerG);

    m_transG->addChild(m_transR);
    m_transG->addChild(m_draggerR);

    m_transR->addChild(m_fixture);
    m_transR->addChild(m_transPan);

    m_transPan->addChild(m_transTilt);
    m_transTilt->addChild(m_lightConeGeode);

    m_draggerG->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 8.0f, 7.0f)));
    m_draggerR->setMatrix(osg::Matrix::rotate( osg::PI / 8, osg::Vec3d(-1, 0, 0)));
    m_transG->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 8.0f, 7.0f)));
    m_transR->setMatrix(osg::Matrix::rotate( osg::PI / 8, osg::Vec3d(-1, 0, 0)));

}

void Fixture3d::updateValues(QByteArray const & ua)
{
    foreach(MonitorFixtureHead *head, m_heads)
    {
        QColor col = head->computeColor(ua);
        setColor(osg::Vec3(col.redF(), col.greenF(), col.blueF()), true);
        
        setOpacity(head->computeAlpha(ua)/255.0, true);

        if (head->hasPan())
        {
            setPan(head->computePanPosition(ua));
        }

        if (head->hasTilt())
        {
            setTilt(head->computeTiltPosition(ua));
        }
        break; // currently supports only one head
    }
}

void Fixture3d::setColor(osg::Vec3 const & colorValue, bool overwrite)
{
    if (m_colors)
    {
        float maxValue = ((colorValue.x() > colorValue.y()) ? colorValue.x() : colorValue.y());
        maxValue = ((maxValue > colorValue.z()) ? maxValue : colorValue.z());
        m_invisibleKolor = 1.0 - maxValue;
        for (unsigned int i = 0; i < m_colors->size(); ++i )
        {
            osg::Vec4 *color = &m_colors->operator [](i);
            if (overwrite)
            {
                color->x() = colorValue.x() + m_invisibleKolor;
                color->y() = colorValue.y() + m_invisibleKolor;
                color->z() = colorValue.z() + m_invisibleKolor;
            }
            else{
                color->x() += colorValue.x();
                color->y() += colorValue.y();
                color->z() += colorValue.z();
            }
        }
    }
}

void Fixture3d::setOpacity(float opacityValue, bool overwrite)
{
    osg::Vec4 *color = &m_colors->operator [](0);

    if (overwrite)
    {
        color->w() = opacityValue - m_invisibleKolor;
    }
    else{
        color->w() = opacityValue - m_invisibleKolor + m_alpha;
    }
}

void Fixture3d::setPan(double angle)
{
    m_transPan->setMatrix(osg::Matrix::rotate(osg::inDegrees(angle), osg::Z_AXIS));
}

void Fixture3d::setTilt(double angle)
{
    m_transTilt->setMatrix(osg::Matrix::rotate(osg::inDegrees(angle), osg::X_AXIS));
}

void Fixture3d::setPosition(double posX, double posY, double posZ)
{
    osg::Matrixd translation = osg::Matrix::translate(osg::Vec3(posX, posY, posZ));
    m_draggerG->setMatrix(translation);
    m_transG->setMatrix(translation);
}

void Fixture3d::setRotation(double rotX, double rotY, double rotZ)
{
    osg::Matrixd rotation = osg::Matrix::rotate(rotX, osg::X_AXIS, rotY, osg::Y_AXIS, rotZ, osg::Z_AXIS);
    m_draggerR->setMatrix(rotation);
    m_transR->setMatrix(rotation);
}

void Fixture3d::setDraggerGVisibility(bool visible)
{
    if (m_visibleG == visible)
        return;

    m_visibleG = visible;
    if (m_visibleG)
    {
        m_draggerG->setNodeMask(~0);
        setDraggerRVisibility(false);
    }
    else
    {
        m_draggerG->setNodeMask(0);
    }
    m_draggerG->setHandleEvents(m_visibleG);
}

void Fixture3d::setDraggerRVisibility(bool visible)
{
    if (m_visibleR == visible)
        return;

    m_visibleR = visible;
    if (m_visibleR)
    {
        m_draggerR->setNodeMask(~0);
        setDraggerGVisibility(false);
    }
    else{
        m_draggerR->setNodeMask(0);
    }
    m_draggerR->setHandleEvents(m_visibleR);
}
