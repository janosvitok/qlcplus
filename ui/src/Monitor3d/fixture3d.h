#ifndef FIXTURE3D_H
#define FIXTURE3D_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <qbytearray.h>

#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/TrackballDragger>

class Fixture3d
{
public:
    Fixture3d(quint32 fixID);
    osg::ref_ptr<osg::Group> getFixture(){ return _transG; }
    osg::ref_ptr<osg::Drawable> getDrawable() {return _lightConeGeode->getDrawable(0); }
    quint32 getID() { return _fixtureID; }

    void changeColor(osg::Vec3 colorValue, bool overwrite = 1 );
    void changeOpacity( float opacityValue, bool overwrite = 1 );

    void setDraggerGVisibility(bool visible);
    void setDraggerRVisibility(bool visible);

private:
    osg::ref_ptr<osg::Vec4Array> _colors;
    osg::ref_ptr<osg::Geode> _lightConeGeode;
//    osg::ref_ptr<osg::Group> _fixture;

    osg::ref_ptr<osg::MatrixTransform> _transG;
    osg::ref_ptr<osg::MatrixTransform> _transR;
    osg::ref_ptr<osg::MatrixTransform> _transQLC;

    osg::ref_ptr<osgManipulator::TranslateAxisDragger> _draggerG;
    osg::ref_ptr<osgManipulator::TrackballDragger> _draggerR;
    bool _visibleG;
    bool _visibleR;

    quint32 _fixtureID;
    float _invisibleKolor; //simmilar as K in CMYK -> to avoid visible gray or black light
    float _alpha;


};

#endif // FIXTURE3D_H
