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
   
    osg::ref_ptr<osg::Group> getFixture() { return m_root; }
    bool contains(osg::Drawable * shape) const;

    void updateValues(QByteArray const & ua);

    void setColor(int head, osg::Vec3 const & colorValue, bool overwrite = 1);
    void setOpacity(int head, float opacityValue, bool overwrite = 1);
    void setPan(double angle);
    void setTilt(double angle);
    void setPosition(double posX, double posY, double posZ);
    void setRotation(double rotX, double rotY, double rotZ, double rotW);

    void getPosition(double & posX, double & posY, double & posZ);
    void getRotation(double & rotX, double & rotY, double & rotZ, double & rotW);

    void setDraggerGVisibility(bool visible);
    void setDraggerRVisibility(bool visible);

private:
    void createParCan();
    void createMovingHead();
    void createLedBar();
    void createLightBeam();
    void createLightSquare(const osg::Vec3 &origin, float x, float y, float angle);
    void createFixture();

private:
    QList<osg::ref_ptr<osg::Vec4Array> > m_colors;
    osg::ref_ptr<osg::Group> m_root;
    QList<osg::ref_ptr<osg::Geode> > m_lightConeGeodes;

    osg::ref_ptr<osg::MatrixTransform> m_transG;
    osg::ref_ptr<osg::MatrixTransform> m_transR;
    osg::ref_ptr<osg::MatrixTransform> m_transPan;
    osg::ref_ptr<osg::MatrixTransform> m_transTilt;
    osg::ref_ptr<osg::MatrixTransform> m_transHead;

    osg::ref_ptr<osgManipulator::TranslateAxisDragger> m_draggerG;
    osg::ref_ptr<osgManipulator::TrackballDragger> m_draggerR;
    bool m_visibleG;
    bool m_visibleR;

    QList<float> m_invisibleKolor; //simmilar as K in CMYK -> to avoid visible gray or black light
    QList<float> m_alpha;
};

#endif // FIXTURE3D_H
