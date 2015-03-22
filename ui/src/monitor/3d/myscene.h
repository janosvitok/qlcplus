#ifndef MYSCENE_H
#define MYSCENE_H

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <qlist.h>
#include <qmutex.h>
#include <qbytearray.h>

#include "fixture3d.h"


class MyScene : public QObject
{
    Q_OBJECT;
public:
    MyScene();
    ~MyScene();
     
    osg::ref_ptr<osg::Group> getRoot() { return _root; }
    QList<Fixture3d *> getFixtures() { return _fixtures3d; }

//    void doUserOperation(osg::Drawable *shape); //raz, ked bude hmatatelny priestor

    void setFixturesChanges();

    void update(const QByteArray &ua);

    void setToBeMovable(osg::Drawable *shape);
    void setToBeRotatable(osg::Drawable *shape);
    void addFixture(Doc * doc, quint32 fid);

private:
    osg::ref_ptr<osg::Group> _root;
    QList<Fixture3d*> _fixtures3d;
    float _stageWidth;
    float _stageHeight;
    float _stageDepth;

    QByteArray _ua;
    bool _changed;
    QMutex _mutex;
};

#endif // MYSCENE_H
