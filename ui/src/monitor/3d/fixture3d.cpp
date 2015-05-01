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
    , m_invisibleKolor()
    , m_alpha()
{
    initialize();
    createFixture();
}

MonitorFixtureHead * Fixture3d::createHead(Fixture & fixture, int head)
{
    return new MonitorFixtureHead(fixture, head);
}

bool Fixture3d::contains(osg::Drawable * shape) const
{
    osg::Node const * node = shape->getParent(0);

    while (node != NULL)
    {
        if (node == m_root)
            return true;
        node = node->getParent(0);
    }
    return false;
}

void Fixture3d::createParCan()
{
    osg::Node * head = osgDB::readNodeFile(
        (QLCFile::systemDirectory(MODELSDIR).path() + QDir::separator() + "PAR64.osgt").toLatin1().constData());

    createLightBeam();

    m_transR->addChild(head);

    foreach(osg::ref_ptr<osg::Geode> lightGeode, m_lightConeGeodes)
    {
        m_transR->addChild(lightGeode);
    }

    m_transPan = m_transR;
    m_transTilt = m_transR;
}

void Fixture3d::createMovingHead()
{
    osg::Node * head = osgDB::readNodeFile(
        (QLCFile::systemDirectory(MODELSDIR).path() + QDir::separator() + "moving_fixture-head.osgt").toLatin1().constData());
    osg::Node * yoke = osgDB::readNodeFile(
        (QLCFile::systemDirectory(MODELSDIR).path() + QDir::separator() + "moving_fixture-yoke.osgt").toLatin1().constData());
    osg::Node * base = osgDB::readNodeFile(
        (QLCFile::systemDirectory(MODELSDIR).path() + QDir::separator() + "moving_fixture-base.osgt").toLatin1().constData());

    createLightBeam();

    m_transR->addChild(base);
    m_transR->addChild(m_transPan);
    m_transPan->addChild(yoke);
    m_transPan->addChild(m_transTilt);
    m_transTilt->addChild(head);

    foreach(osg::ref_ptr<osg::Geode> lightGeode, m_lightConeGeodes)
    {
        m_transTilt->addChild(lightGeode);
    }
}

void Fixture3d::createLedBar()
{
    osg::Node * bar = osgDB::readNodeFile(
        (QLCFile::systemDirectory(MODELSDIR).path() + QDir::separator() + "ledBar.osgt").toLatin1().constData());

    //sirka ledBar je 965 mm. hlavy treba posuvat z = 0.01, x o 0.12 + rozpocitat zvysnych 5mm. Mozno je opacne pomer sirka/vyska

    for (int h = 0; h < m_heads.size(); ++h)
    {
        createLightSquare(osg::Vec3(0.06f + 0.12f * (h - 4), 0.0f, 0.01f), 0.12f, 0.06f, osg::inDegrees(10.0f));
    }

    m_transR->addChild(m_transPan);
    m_transPan->addChild(m_transTilt);
    m_transTilt->addChild(bar);
    foreach(osg::ref_ptr<osg::Geode> lightGeode, m_lightConeGeodes)
    {
        m_transTilt->addChild(lightGeode);
    }
}

void Fixture3d::createLightBeam()
{
    osg::ref_ptr<osg::Geode> lightConeGeode = new osg::Geode();
    m_lightConeGeodes.push_back(lightConeGeode);

    osg::ref_ptr<osg::Geometry> pyramidGeometry = new osg::Geometry();
    lightConeGeode->addDrawable(pyramidGeometry);
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
        pyramidVertices->push_back(osg::Vec3(sin(position) * width, cos(position) * height, (lenght))); // points at base
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

    osg::Vec4Array * colors = new osg::Vec4Array();
    m_colors.push_back(colors);
    m_invisibleKolor.push_back(0);
    m_alpha.push_back(0);
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 0 white
    for (int i = 0; i < faces; ++i)
    {
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f) ); //index i off
    }

    pyramidGeometry->setColorArray(colors);
    pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::StateSet* stateset = lightConeGeode->getOrCreateStateSet();
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stateset->setAttributeAndModes(blendFunc);

    osg::CullFace* cull = new osg::CullFace();
    cull->setMode(osg::CullFace::BACK);
    stateset->setAttributeAndModes(cull, osg::StateAttribute::ON);
}

void Fixture3d::createLightSquare(osg::Vec3 const & origin, float x, float y, float angle)
{
    osg::ref_ptr<osg::Geode> lightConeGeode = new osg::Geode();
    m_lightConeGeodes.push_back(lightConeGeode);

    osg::ref_ptr<osg::Geometry> pyramidGeometry = new osg::Geometry();
    lightConeGeode->addDrawable(pyramidGeometry);
    pyramidGeometry->setDataVariance(osg::Object::DYNAMIC);
    pyramidGeometry->setUseDisplayList(false);

    osg::ref_ptr<osg::Vec3Array> pyramidVertices = new osg::Vec3Array();

    float length = 1;
    float d = tan(angle) * length;

    pyramidVertices->push_back(origin + osg::Vec3(-x/2.0f,     -y/2.0f,     y));  //1
    pyramidVertices->push_back(origin + osg::Vec3(-x/2.0f - d, -y/2.0f - d, y + length));  //2
    pyramidVertices->push_back(origin + osg::Vec3( x/2.0f,     -y/2.0f,     y));  //3
    pyramidVertices->push_back(origin + osg::Vec3( x/2.0f + d, -y/2.0f - d, y + length));  //4
    pyramidVertices->push_back(origin + osg::Vec3( x/2.0f,      y/2.0f,     y));  //5
    pyramidVertices->push_back(origin + osg::Vec3( x/2.0f + d,  y/2.0f + d, y + length));  //6
    pyramidVertices->push_back(origin + osg::Vec3(-x/2.0f,      y/2.0f,     y));  //7
    pyramidVertices->push_back(origin + osg::Vec3(-x/2.0f - d,  y/2.0f + d, y + length));  //8

    pyramidGeometry->setVertexArray(pyramidVertices);

    //cone faces creation
    osg::ref_ptr<osg::DrawElementsUInt> pyramidFace =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP, 0);
    for (size_t i = 0; i < pyramidVertices->size(); ++i)
    {
        pyramidFace->push_back(i);
    }

    pyramidFace->push_back(0);
    pyramidFace->push_back(1);

    pyramidGeometry->addPrimitiveSet(pyramidFace);

    osg::Vec4Array * colors = new osg::Vec4Array();
    m_colors.push_back(colors);
    m_invisibleKolor.push_back(0);
    m_alpha.push_back(0);

    for (size_t i = 0; i <= pyramidVertices->size(); ++i)
    {
        if (i % 2 == 0)
            colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); // odd white
        else
            colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f) ); // even off
    }

    pyramidGeometry->setColorArray(colors);
    pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    osg::StateSet* stateset = lightConeGeode->getOrCreateStateSet();
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
    //all fixture transformations
    m_transG = new osg::MatrixTransform;

    m_transR = new osg::MatrixTransform;
    m_transR->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    m_transPan = new osg::MatrixTransform;
    m_transPan->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    m_transTilt = new osg::MatrixTransform;
    m_transTilt->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);


    m_draggerG = new osgManipulator::TranslateAxisDragger();
    m_draggerG->setupDefaultGeometry();
    m_draggerG->setHandleEvents(false);
    m_draggerG->addTransformUpdating(m_transG);
    m_draggerG->setNodeMask(0);

    m_draggerR = new osgManipulator::TrackballDragger();
    m_draggerR->setupDefaultGeometry();

    m_draggerR->setHandleEvents(false);
    m_draggerR->addTransformUpdating(m_transR);
    m_draggerR->setNodeMask(0);
    m_draggerR->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);


    /* ak by sa chcelo riesit pridanie viacerych hlav,
     * treba pridat na zaciatku este 1 trasformaciu,
     * ktora by posunula jednotlive hlavy vedla seba,
     * aby bolo mozne posuvat a inak nimi manipulovat rucne vsetkymi spolu.
     * Len treba dodat info o ich vzajomnej vzdialenosti
     */

    //depending on flexibility of the head get the right one
    if (m_heads.size() == 8)
    {
        createLedBar();
    }
    else if (m_heads[0]->hasPan() || m_heads[0]->hasTilt())
    {
        createMovingHead();
    }
    else
    {
        createParCan();
    }

    m_root = new osg::Group();

    m_root->addChild(m_transG);
    m_root->addChild(m_draggerG);

    m_transG->addChild(m_transR);
    m_transG->addChild(m_draggerR);

    m_draggerG->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 8.0f, 7.0f)));
    //m_draggerR->setMatrix(osg::Matrix::rotate( osg::PI * 7 / 8, osg::Vec3d(1, 0, 0)));
    m_transG->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 8.0f, 7.0f)));
    //m_transR->setMatrix(osg::Matrix::rotate( osg::PI * 7 / 8, osg::Vec3d(1, 0, 0)));
}

void Fixture3d::updateValues(QByteArray const & ua)
{
    for(int h = 0; h < m_heads.size(); ++h)
    {
        MonitorFixtureHead *head = m_heads[h];
        QColor col = head->computeColor(ua);
        setColor(h, osg::Vec3(col.redF(), col.greenF(), col.blueF()), true);
        
        setOpacity(h, head->computeAlpha(ua)/255.0, true);

        if (head->hasPan())
        {
            setPan(head->computePanPosition(ua));
        }

        if (head->hasTilt())
        {
            setTilt(head->computeTiltPosition(ua));
        }
    }
}

void Fixture3d::setColor(int head, osg::Vec3 const & colorValue, bool overwrite)
{
    if (m_colors.size() <= head || m_colors[head] == NULL)
        return;

    float maxValue = ((colorValue.x() > colorValue.y()) ? colorValue.x() : colorValue.y());
    maxValue = ((maxValue > colorValue.z()) ? maxValue : colorValue.z());
    m_invisibleKolor[head] = 1.0 - maxValue;
    osg::Vec4Array & colors = *m_colors[head];

    for (unsigned int i = 0; i < colors.size(); ++i )
    {
        osg::Vec4 & color = colors[i];
        if (overwrite)
        {
            color.x() = colorValue.x() + m_invisibleKolor[head];
            color.y() = colorValue.y() + m_invisibleKolor[head];
            color.z() = colorValue.z() + m_invisibleKolor[head];
        }
        else
        {
            color.x() += colorValue.x();
            color.y() += colorValue.y();
            color.z() += colorValue.z();
        }
    }
}

void Fixture3d::setOpacity(int head, float opacityValue, bool overwrite)
{
    if (m_colors.size() < head || m_colors[head] == NULL)
        return;

    osg::Vec4 & color = m_colors[head]->operator [](0);

    if (overwrite)
    {
        color.w() = opacityValue - m_invisibleKolor[head];
    }
    else
    {
        color.w() = opacityValue - m_invisibleKolor[head] + m_alpha[head];
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

void Fixture3d::setRotation(double rotX, double rotY, double rotZ, double rotW)
{
    osg::Quat q(rotX, rotY, rotZ, rotW);
    osg::Matrixd rotation = osg::Matrix::rotate(q);
    m_draggerR->setMatrix(rotation);
    m_transR->setMatrix(rotation);
}

void Fixture3d::getPosition(double & posX, double & posY, double & posZ)
{
    osg::Vec3d trans = m_transG->getMatrix().getTrans();
    
    posX = trans.x();
    posY = trans.y();
    posZ = trans.z();
}

void Fixture3d::getRotation(double & rotX, double & rotY, double & rotZ, double & rotW)
{
    osg::Quat rot = m_transR->getMatrix().getRotate();

    rotX = rot.x();
    rotY = rot.y();
    rotZ = rot.z();
    rotW = rot.w();
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
    else
    {
        m_draggerR->setNodeMask(0);
    }
    m_draggerR->setHandleEvents(m_visibleR);
}
