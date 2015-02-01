#ifndef MYSCENE_H
#define MYSCENE_H

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <qlist.h>

#include "fixture3d.h"


class MyScene
{
public:
    MyScene();
    osg::ref_ptr<osg::Group> get(){return _root;}
    QList<Fixture3d> getCone(){ return _fixture3d; }

//    void doUserOperation(osg::Drawable *shape); //raz, ked bude hmatatelny priestor

private:
    osg::ref_ptr<osg::Group> _root;
    QList<Fixture3d> _fixture3d;
    float _stageWidth;
    float _stageHeight;
    float _stageDepth;


};

#endif // MYSCENE_H
