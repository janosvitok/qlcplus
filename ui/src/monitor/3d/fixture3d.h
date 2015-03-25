#ifndef FIXTURE3D_H
#define FIXTURE3D_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <qbytearray.h>

#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/TrackballDragger>

#include "monitorfixturebase.h"

class QByteArray;

class Fixture3d : MonitorFixtureBase
{
    Q_OBJECT;
public:
    Fixture3d(Doc * doc, quint32 fid);

    MonitorFixtureHead * createHead(Fixture & fixture, int head);
   
    osg::ref_ptr<osg::Group> getFixture(){ return _transG; }
    osg::ref_ptr<osg::Drawable> getDrawable() {return _lightConeGeode->getDrawable(0); }

    void updateValues(QByteArray const & ua);

    void setColor(osg::Vec3 const & colorValue, bool overwrite = 1);
    void setOpacity(float opacityValue, bool overwrite = 1);
    void setPan(double angle);
    void setTilt(double angle);
    void setPosition(double posX, double posY, double posZ);
    void setRotation(double rotX, double rotY, double rotZ);

    void setDraggerGVisibility(bool visible);
    void setDraggerRVisibility(bool visible);

private:
    void createParCan();

private:
    osg::ref_ptr<osg::Vec4Array> _colors;
    osg::ref_ptr<osg::Geode> _lightConeGeode;
//    osg::ref_ptr<osg::Group> _fixture;

    osg::ref_ptr<osg::MatrixTransform> _transG;
    osg::ref_ptr<osg::MatrixTransform> _transR;
    osg::ref_ptr<osg::MatrixTransform> _transPan;
    osg::ref_ptr<osg::MatrixTransform> _transTilt;

    osg::ref_ptr<osgManipulator::TranslateAxisDragger> _draggerG;
    osg::ref_ptr<osgManipulator::TrackballDragger> _draggerR;
    bool _visibleG;
    bool _visibleR;

    float _invisibleKolor; //simmilar as K in CMYK -> to avoid visible gray or black light
    float _alpha;
};

#endif // FIXTURE3D_H
