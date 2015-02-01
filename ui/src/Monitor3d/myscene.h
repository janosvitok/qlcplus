#ifndef MYSCENE_H
#define MYSCENE_H

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <qlist.h>


class MyScene
{
public:
    MyScene();
    osg::ref_ptr<osg::Group> get(){return _root;}
//    QList<Cone> getCone(){ return _cone; }

//    void doUserOperation(osg::Drawable *shape); //raz, ked bude hmatatelny priestor

private:
    osg::ref_ptr<osg::Group> _root;
//    QList<Cone> _cone;

};

#endif // MYSCENE_H
