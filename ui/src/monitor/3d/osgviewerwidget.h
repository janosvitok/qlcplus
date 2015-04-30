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
    OsgViewerWidget(QWidget *parent, osgQt::GraphicsWindowQt* gw, OsgScene *scene);

    void setCameraPosition(float eyeX,    float eyeY,    float eyeZ,
                           float centerX, float centerY, float centerZ,
                           float upX,     float upY,     float upZ);

    void getCameraPosition(float & eyeX,    float & eyeY,    float & eyeZ,
                           float & centerX, float & centerY, float & centerZ,
                           float & upX,     float & upY,     float & upZ);

signals:

public slots:

protected:
    virtual void paintEvent(QPaintEvent*);

private:
    OsgScene *m_scene;
    osgViewer::Viewer m_viewer;
    QTimer m_timer;
};

#endif // OSGVIEWERWIDGET_H
