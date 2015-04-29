#include "osgviewerwidget.h"

#include <QVBoxLayout>

#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osgViewer/ViewerEventHandlers>
#include "pickhandler.h"

OsgViewerWidget::OsgViewerWidget(QWidget * parent, osgQt::GraphicsWindowQt *gw, MyScene *scene) 
    : QWidget(parent)
    , m_scene(scene)
{
    const osg::GraphicsContext::Traits* traits = gw->getTraits();
    osg::Camera* camera = m_viewer.getCamera();
    camera->setGraphicsContext(gw);
    camera->setClearColor(osg::Vec4(0.2, 0.2, 0.6, 1.0));
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f);

    m_viewer.setSceneData(m_scene->getRoot());
//    m_viewer.addEventHandler(new osgViewer::StatsHandler);
    m_viewer.addEventHandler(new PickHandler(m_scene));
    m_viewer.setCameraManipulator(new osgGA::TrackballManipulator);
    m_viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(gw->getGLWidget());
    setLayout(layout);

    connect( &m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.start(40);
}

void OsgViewerWidget::paintEvent(QPaintEvent*)
{
    m_viewer.frame();
}

void OsgViewerWidget::setCameraPosition(double posX, double posY, double posZ)
{
    static_cast<osgGA::TrackballManipulator*>(m_viewer.getCameraManipulator())->setCenter(osg::Vec3(posX, posY, posZ));
}

void OsgViewerWidget::setCameraRotation(double rotX, double rotY, double rotZ, double rotW)
{
    static_cast<osgGA::TrackballManipulator*>(m_viewer.getCameraManipulator())->setRotation(osg::Quat(rotX, rotY, rotZ, rotW));
}

void OsgViewerWidget::getCameraPosition(double & posX, double & posY, double & posZ)
{
    osg::Vec3d const & trans = static_cast<osgGA::TrackballManipulator*>(m_viewer.getCameraManipulator())->getCenter();

    posX = trans.x();
    posY = trans.y();
    posZ = trans.z();
}

void OsgViewerWidget::getCameraRotation(double & rotX, double & rotY, double & rotZ, double & rotW)
{
    osg::Quat const & rot = static_cast<osgGA::TrackballManipulator*>(m_viewer.getCameraManipulator())->getRotation();

    rotX = rot.x();
    rotY = rot.y();
    rotZ = rot.z();
    rotW = rot.w();
}
