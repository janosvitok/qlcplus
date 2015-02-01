#include "dialog3d.h"
#include "ui_dialog3d.h"

#include <osg/Geode>

#include "osgviewerwidget.h"
#include "myscene.h"

osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h )
{
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowDecoration = false;
    traits->x = x;
    traits->y = y;
    traits->width = w;
    traits->height = h;
    traits->doubleBuffer = true;

    return new osgQt::GraphicsWindowQt(traits.get());
}

Dialog3d::Dialog3d(Doc *doc, QWidget *parent) :
    QDialog(parent),
    _doc(doc),
    ui(new Ui::Dialog3d)

{
    ui->setupUi(this);

    osgQt::GraphicsWindowQt* gw = createGraphicsWindow( 50, 50, 640, 480 );
    MyScene* root = new MyScene();
    OsgViewerWidget* widget = new OsgViewerWidget(gw, root );
    widget->setGeometry( 100, 100, 800, 600 );
    widget->show();

}


Dialog3d::~Dialog3d()
{
    delete ui;
}
