#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <QWidget>
#include <QTimer>
#include <osg/Geode>
#include <osgViewer/Viewer>
#include <osgQt/GraphicsWindowQt>
#include "myscene.h"


class OsgViewerWidget : public QWidget
{
    Q_OBJECT
public:
    OsgViewerWidget(osgQt::GraphicsWindowQt* gw, MyScene *scene );

signals:

public slots:

protected:
    virtual void paintEvent( QPaintEvent* )
    { _viewer.frame(); }

    osgViewer::Viewer _viewer;
    QTimer _timer;

};

#endif // OSGVIEWERWIDGET_H
