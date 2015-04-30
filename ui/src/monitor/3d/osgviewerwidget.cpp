#include "osgviewerwidget.h"

#include <QVBoxLayout>

#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osgViewer/ViewerEventHandlers>
#include "pickhandler.h"

OsgViewerWidget::OsgViewerWidget(QWidget * parent, osgQt::GraphicsWindowQt *gw, OsgScene *scene) 
    : QWidget(parent)
    , m_scene(scene)
{
    const osg::GraphicsContext::Traits* traits = gw->getTraits();
    osg::Camera* camera = m_viewer.getCamera();
    camera->setGraphicsContext(gw);
    camera->setClearColor(osg::Vec4(0.125f, 0.117f, 0.137f, 1.f ));
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

void OsgViewerWidget::setCameraPosition(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
{

    osg::Vec3f eye( eyeX, eyeY, eyeZ );
    osg::Vec3f center( centerX, centerY, centerZ );
    osg::Vec3f up( upX, upY, upZ );

    m_viewer.getCamera()->setViewMatrixAsLookAt( eye, center, up );
}

void OsgViewerWidget::getCameraPosition(float &eyeX, float &eyeY, float &eyeZ, float &centerX, float &centerY, float &centerZ, float &upX, float &upY, float &upZ)
{
    osg::Vec3f eye( eyeX, eyeY, eyeZ );
    osg::Vec3f center( centerX, centerY, centerZ );
    osg::Vec3f up( upX, upY, upZ );

    m_viewer.getCamera()->getViewMatrixAsLookAt( eye, center, up );

}


