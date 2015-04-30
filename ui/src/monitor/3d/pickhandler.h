#ifndef PICKHANDLER_H
#define PICKHANDLER_H

#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>
#include "myscene.h"

class PickHandler : public osgGA::GUIEventHandler
{
public:
    PickHandler(OsgScene * scene);
    bool handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa);

private:
    OsgScene * m_scene;
    void doUserOperations(
        osgUtil::LineSegmentIntersector::Intersection & result,
        const osgGA::GUIEventAdapter & ea);
};

#endif // PICKHANDLER_H
