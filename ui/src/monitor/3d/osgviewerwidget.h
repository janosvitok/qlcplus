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
    OsgViewerWidget(QWidget *parent, osgQt::GraphicsWindowQt* gw, MyScene *scene);

signals:

public slots:

protected:
    virtual void paintEvent(QPaintEvent*);

private:
    MyScene *m_scene;
    osgViewer::Viewer m_viewer;
    QTimer m_timer;
};

#endif // OSGVIEWERWIDGET_H
