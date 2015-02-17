#include "dialog3d.h"
#include "ui_dialog3d.h"

#include <osg/Geode>
#include <qbytearray.h>

#include "osgviewerwidget.h"
#include "myscene.h"
#include "fixture.h"
#include "fixtureselection.h"

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

void Dialog3d::slotUniversesWritten(int index, const QByteArray &ua)
{
    if(index != 0){
        return;
    }
    _scene->update(ua);
}

Dialog3d::Dialog3d(QWidget *parent, Doc *doc) :
    QDialog(parent),
    _doc(doc),
    ui(new Ui::Dialog3d)

{
    ui->setupUi(this);

    osgQt::GraphicsWindowQt* gw = createGraphicsWindow( 50, 50, 640, 480 );
    _scene = new MyScene();
    OsgViewerWidget* widget = new OsgViewerWidget(gw, _scene );
    widget->setGeometry( 100, 100, 800, 600 );
    widget->show();
    connect(_doc->inputOutputMap(), SIGNAL(universesWritten(int, const QByteArray&)),
            this, SLOT(slotUniversesWritten(int, const QByteArray&)));


}


Dialog3d::~Dialog3d()
{
    delete ui;
}

void Dialog3d::on_pushButton_clicked()
{
    /* Get a list of new fixtures to add to the scene */
    FixtureSelection fs(this, _doc);
    fs.setMultiSelection(true);
    if (fs.exec() == QDialog::Accepted)
    {
        QListIterator <quint32> it(fs.selection());
        while (it.hasNext() == true)
        {
            quint32 fid = it.next();
            _scene->addFixture(fid);
            _doc->setModified();
        }
    }
}

void Dialog3d::on_pushButton_2_clicked()
{
        int numberOfFixtures = 15;
        float stageWidth = 15.0;
        float space = stageWidth / (float)(numberOfFixtures + 1);
        float posX = stageWidth * -0.5f;
        for( int i = 0; i < numberOfFixtures; ++i){
            _scene->addFixture(i);
            posX += space;
            _scene->getFixtures().last().changeColor(osg::Vec3((float)(i+ 1)/(float)numberOfFixtures, 1.0f, 0.0f));
            _scene->getFixtures().last().moveHead(posX, 4.0f + (float)i / 4, 4.0f);
            _scene->getFixtures().last().changeOpacity(0.6);
        }
}
