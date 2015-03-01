#ifndef PICKHANDLER_H
#define PICKHANDLER_H

#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>
#include "myscene.h"

class PickHandler : public osgGA::GUIEventHandler
{
public:
    PickHandler( MyScene * scene ) : _scene( scene ) {}
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

private:
    MyScene *_scene;
    void doUserOperations( osgUtil::LineSegmentIntersector::Intersection& result,
                                   const osgGA::GUIEventAdapter& ea );
};

#endif // PICKHANDLER_H
