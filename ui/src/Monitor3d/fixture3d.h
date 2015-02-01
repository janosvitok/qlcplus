#ifndef FIXTURE3D_H
#define FIXTURE3D_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

class Fixture3d
{
public:
    Fixture3d();
//    osg::ref_ptr<osg::Geode> getCone(){ return _pyramidGeode; }
    osg::ref_ptr<osg::Group> getFixture(){ return _fixture; }
    void changeColor(osg::Vec3 colorValue, bool overwrite = 1 );
    void changeOpacity( float opacityValue, bool overwrite = 1 );
private:
    osg::ref_ptr<osg::Vec4Array> _colors;
    osg::ref_ptr<osg::Geode> _pyramidGeode;
    osg::ref_ptr<osg::Group> _fixture;


};

#endif // FIXTURE3D_H
