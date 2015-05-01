#include "object3d.h"
#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include <osg/CullFace>

#include "qlcfile.h"
#include "qlcconfig.h"

#include <QDebug>

Object3d::Object3d(QString const & modelPath)
    : m_visibleG(false)
    , m_visibleR(false)
    , m_modelPath(modelPath)
{
    //all fixture transformations
    m_transG = new osg::MatrixTransform;

    m_transR = new osg::MatrixTransform;
    m_transR->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

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

    osg::Node * model = osgDB::readNodeFile(m_modelPath.toLatin1().constData());

    m_root = new osg::Group();

    m_root->addChild(m_transG);
    m_root->addChild(m_draggerG);

    m_transG->addChild(m_transR);
    m_transG->addChild(m_draggerR);

    m_draggerG->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 8.0f, 7.0f)));
    m_transG->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 8.0f, 7.0f)));

    m_transR->addChild(model);
}

bool Object3d::contains(osg::Drawable * shape) const
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

void Object3d::setPosition(double posX, double posY, double posZ)
{
    osg::Matrixd translation = osg::Matrix::translate(osg::Vec3(posX, posY, posZ));
    m_draggerG->setMatrix(translation);
    m_transG->setMatrix(translation);
}

void Object3d::setRotation(double rotX, double rotY, double rotZ, double rotW)
{
    osg::Quat q(rotX, rotY, rotZ, rotW);
    osg::Matrixd rotation = osg::Matrix::rotate(q);
    m_draggerR->setMatrix(rotation);
    m_transR->setMatrix(rotation);
}

void Object3d::getPosition(double & posX, double & posY, double & posZ)
{
    osg::Vec3d trans = m_transG->getMatrix().getTrans();
    
    posX = trans.x();
    posY = trans.y();
    posZ = trans.z();
}

void Object3d::getRotation(double & rotX, double & rotY, double & rotZ, double & rotW)
{
    osg::Quat rot = m_transR->getMatrix().getRotate();

    rotX = rot.x();
    rotY = rot.y();
    rotZ = rot.z();
    rotW = rot.w();
}

void Object3d::setDraggerGVisibility(bool visible)
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

void Object3d::setDraggerRVisibility(bool visible)
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
