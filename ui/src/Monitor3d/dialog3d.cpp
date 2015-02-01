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

Dialog3d* Dialog3d::s_instance = NULL;

Dialog3d *Dialog3d::instance()
{
    return s_instance;
}

void Dialog3d::createAndShow(QWidget *parent, Doc *doc)
{
    QDialog* window = NULL;

    /* Must not create more than one instance */
    if (s_instance == NULL)
    {
        /* Create a separate window for OSX */
        s_instance = new Dialog3d(parent, doc);
        window = s_instance;

//        /* Set some common properties for the window and show it */
//        window->setAttribute(Qt::WA_DeleteOnClose);
//        window->setWindowIcon(QIcon(":/monitor.png"));
//        window->setWindowTitle(tr("Fixture Monitor"));
//        window->setContextMenuPolicy(Qt::CustomContextMenu);

//        QSettings settings;
//        QVariant var = settings.value(SETTINGS_GEOMETRY);
//        if (var.isValid() == true)
//            window->restoreGeometry(var.toByteArray());
//        else
//        {
//            window->resize(800, 600);
//            window->move(50, 50);
//        }
//        AppUtil::ensureWidgetIsVisible(window);
    }
    else
    {
        window = s_instance;
    }

    window->show();
    window->raise();
}

Dialog3d::Dialog3d(QWidget *parent, Doc *doc) :
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
