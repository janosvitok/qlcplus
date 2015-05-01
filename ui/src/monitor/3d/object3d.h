#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/TrackballDragger>

#include <QObject>

class Object3d : public QObject
{
    Q_OBJECT;
public:
    Object3d(QString const & modelPath);

    osg::ref_ptr<osg::Group> node() { return m_root; }
    bool contains(osg::Drawable * shape) const;

    void setPosition(double posX, double posY, double posZ);
    void setRotation(double rotX, double rotY, double rotZ, double rotW);

    void getPosition(double & posX, double & posY, double & posZ);
    void getRotation(double & rotX, double & rotY, double & rotZ, double & rotW);

    void setDraggerGVisibility(bool visible);
    void setDraggerRVisibility(bool visible);

    QString const modelPath()const { return m_modelPath; }
private:
    osg::ref_ptr<osg::Group> m_root;

    osg::ref_ptr<osg::MatrixTransform> m_transG;
    osg::ref_ptr<osg::MatrixTransform> m_transR;

    osg::ref_ptr<osgManipulator::TranslateAxisDragger> m_draggerG;
    osg::ref_ptr<osgManipulator::TrackballDragger> m_draggerR;
    bool m_visibleG;
    bool m_visibleR;

    QString m_modelPath;
};

#endif // OBJECT3D_H
