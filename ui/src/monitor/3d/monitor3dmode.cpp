/*
  Q Light Controller Plus
  monitor3dmode.cpp

  Copyright (c) Jano Svitok
  Copyright (c) Betka Svitkova

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <QApplication>
#include <QActionGroup>
#include <QSpacerItem>
#include <QByteArray>
#include <QComboBox>
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QLabel>
#include <QFont>
#include <QIcon>
#include <QtXml>
#include <QFileDialog>

#include <osg/Geode>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

#include "fixture.h"
#include "fixtureselection.h"
#include "universe.h"
#include "monitor.h"
#include "apputil.h"
#include "doc.h"
#include "qlcfile.h"
#include "qlcconfig.h"

#include "monitor3dmode.h"
#include "myscene.h"
#include "pickhandler.h"
#include "osgviewerwidget.h"

#define SETTINGS_3D_VSPLITTER "monitor/3d_vsplitter"

/*****************************************************************************
 * Initialization
 *****************************************************************************/

Monitor3dMode::Monitor3dMode(QWidget* monitor, Doc* doc)
    : MonitorMode(monitor, doc)
    , m_osgWidget(NULL)
{
}

Monitor3dMode::~Monitor3dMode()
{
}

QString Monitor3dMode::name() const
{
    return tr("3D View");
}

MonitorProperties::DisplayMode Monitor3dMode::displayMode() const
{
    return MonitorProperties::ThreeD;
}

void Monitor3dMode::connectSignals()
{
    /* Listen to fixture additions and changes from Doc */
    connect(doc(), SIGNAL(fixtureChanged(quint32)),
        this, SLOT(slotFixtureChanged(quint32)));
    connect(doc(), SIGNAL(fixtureRemoved(quint32)),
        this, SLOT(slotFixtureRemoved(quint32)));

    connect(doc()->inputOutputMap(), SIGNAL(universesWritten(int, const QByteArray&)),
        this, SLOT(slotUniversesWritten(int, const QByteArray&)));
}

void Monitor3dMode::disconnectSignals()
{
    disconnect(doc(), SIGNAL(fixtureChanged(quint32)),
        this, SLOT(slotFixtureChanged(quint32)));
    disconnect(doc(), SIGNAL(fixtureRemoved(quint32)),
        this, SLOT(slotFixtureRemoved(quint32)));

    disconnect(doc()->inputOutputMap(), SIGNAL(universesWritten(int, const QByteArray&)),
        this, SLOT(slotUniversesWritten(int, const QByteArray&)));
}

void Monitor3dMode::saveSettings()
{
    if (m_osgWidget != NULL)
    {
        Camera3dProperties p = props()->cameraProperties();
        m_osgWidget->getCameraPosition( p.m_eyeX, p.m_eyeY, p.m_eyeZ,
                                        p.m_centerX, p.m_centerY, p.m_centerZ,
                                        p.m_upX, p.m_upY, p.m_upZ);
        props()->setCameraProperties(p);
    }

    if (m_scene != NULL)
    {
	foreach(quint32 fid, m_scene->fixtureIds())
        {
            Fixture3dProperties p = props()->fixture3dProperties(fid);
            Fixture3d * fixture = m_scene->getFixture(fid);
            fixture->getPosition(p.m_posX, p.m_posY, p.m_posZ);
            fixture->getRotation(p.m_rotX, p.m_rotY, p.m_rotZ, p.m_rotW);
            props()->setFixture3dProperties(fid, p);
        }
    }
 
    if (m_splitter != NULL)
    {
        QSettings settings;
        settings.setValue(SETTINGS_3D_VSPLITTER, m_splitter->saveState());
    }
}

void Monitor3dMode::initToolBar(QToolBar* toolBar)
{
    m_unitsCombo = new QComboBox();
    m_unitsCombo->addItem(tr("Meters"), MonitorProperties::Meters);
    m_unitsCombo->addItem(tr("Feet"), MonitorProperties::Feet);
    if (props()->gridUnits() == MonitorProperties::Feet)
        m_unitsCombo->setCurrentIndex(1);
    toolBar->addWidget(m_unitsCombo);
    connect(m_unitsCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotGridUnitsChanged(int)));

    toolBar->addSeparator();

    toolBar->addAction(QIcon(":/edit_add.png"), tr("Add fixture"),
        this, SLOT(slotAddFixture()));
    toolBar->addAction(QIcon(":/edit_remove.png"), tr("Remove fixture"),
        this, SLOT(slotRemoveFixture()));

    toolBar->addSeparator();

    m_labelsAction = toolBar->addAction(QIcon(":/label.png"), tr("Show/hide labels"));
    m_labelsAction->setCheckable(true);
    m_labelsAction->setChecked(props()->labelsVisible());
    connect(m_labelsAction, SIGNAL(triggered(bool)),
        this, SLOT(slotShowLabels(bool)));

    toolBar->addSeparator();

    toolBar->addAction(QIcon(":/edit_add.png"), tr("Add object"),
        this, SLOT(slotAddObject()));
}

void Monitor3dMode::destroyToolBar()
{
    if (m_unitsCombo)
    {
        disconnect(m_unitsCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotGridUnitsChanged(int)));
        m_unitsCombo->deleteLater();
        m_unitsCombo = NULL;
    }

    if (m_labelsAction)
    {
        disconnect(m_labelsAction, SIGNAL(triggered(bool)),
            this, SLOT(slotShowLabels(bool)));
        m_labelsAction->deleteLater();
        m_labelsAction = NULL;
    }
}

void Monitor3dMode::initUi()
{
    m_splitter = new QSplitter(Qt::Horizontal, monitor());
    monitor()->layout()->addWidget(m_splitter);
    QWidget* gcontainer = new QWidget(monitor());
    m_splitter->addWidget(gcontainer);
    gcontainer->setLayout(new QVBoxLayout);
    gcontainer->layout()->setContentsMargins(0, 0, 0, 0);

    m_scene = new OsgScene();

    osg::ref_ptr<osg::GraphicsContext::Traits> t = new osg::GraphicsContext::Traits;
    t->windowDecoration = false;
    t->x = 0;
    t->y = 0;
    t->width = gcontainer->width();
    t->height = gcontainer->height();
    t->doubleBuffer = true;

    osgQt::GraphicsWindowQt* gw = new osgQt::GraphicsWindowQt(t.get(), m_splitter->widget(0));
    m_osgWidget = new OsgViewerWidget(m_splitter->widget(0), gw, m_scene);
    m_splitter->widget(0)->layout()->addWidget(m_osgWidget);

    // add container for chaser editor
    QWidget* econtainer = new QWidget(monitor());
    m_splitter->addWidget(econtainer);
    econtainer->setLayout(new QVBoxLayout);
    econtainer->layout()->setContentsMargins(0, 0, 0, 0);
    m_splitter->widget(1)->hide();

    m_fixtureItemEditor = new QWidget(m_splitter->widget(1));
    m_splitter->widget(1)->layout()->addWidget(m_fixtureItemEditor);
    m_splitter->widget(1)->show();
    m_fixtureItemEditor->show();

    QSettings settings;
    QVariant var2 = settings.value(SETTINGS_3D_VSPLITTER);
    if (var2.isValid() == true)
        m_splitter->restoreState(var2.toByteArray());

    {
        Camera3dProperties p = props()->cameraProperties();
        m_osgWidget->setCameraPosition( p.m_eyeX, p.m_eyeY, p.m_eyeZ,
                                        p.m_centerX, p.m_centerY, p.m_centerZ,
                                        p.m_upX, p.m_upY, p.m_upZ);
    }

    foreach(quint32 fid, props()->fixture3dID())
    {
        Fixture3dProperties p = props()->fixture3dProperties(fid);
        m_scene->addFixture(doc(), fid);
        Fixture3d * fixture = m_scene->getFixture(fid);
        fixture->setPosition(p.m_posX, p.m_posY, p.m_posZ);
        fixture->setRotation(p.m_rotX, p.m_rotY, p.m_rotZ, p.m_rotW);
    }

    setMonitorUniverse(Universe::invalid());
}

void Monitor3dMode::destroyUi()
{
    saveSettings();

    if (m_fixtureItemEditor != NULL)
    {
        m_splitter->widget(1)->layout()->removeWidget(m_fixtureItemEditor);
        m_splitter->widget(1)->hide();
        m_fixtureItemEditor = NULL;
    }

    m_scene = NULL;

    if (m_splitter != NULL)
    {
        m_splitter->deleteLater();
        m_splitter = NULL;
    }
}

/****************************************************************************
 * Fixture added/removed stuff
 ****************************************************************************/

void Monitor3dMode::slotFixtureChanged(quint32 fxi_id)
{
    Q_UNUSED(fxi_id);
}

void Monitor3dMode::slotFixtureRemoved(quint32 fxi_id)
{
    props()->removeFixture3d(fxi_id);
    m_scene->removeFixture(fxi_id);
}

void Monitor3dMode::slotUniversesWritten(int index, const QByteArray& ua)
{
    if (m_scene == NULL)
        return;

    if (index != 0)
        return;
    
    m_scene->update(ua);
    m_splitter->widget(0)->update();
}

/********************************************************************
 * 3D View
 ********************************************************************/

void Monitor3dMode::slotGridUnitsChanged(int index)
{
    MonitorProperties::GridUnits units = MonitorProperties::Meters;

    QVariant var = m_unitsCombo->itemData(index);
    if (var.isValid())
        units = MonitorProperties::GridUnits(var.toInt());

    //if (units == MonitorProperties::Meters)
    //    m_graphicsView->setGridMetrics(1000.0);
    //else if (units == MonitorProperties::Feet)
    //    m_graphicsView->setGridMetrics(304.8);

    props()->setGridUnits(units);
}

void Monitor3dMode::slotAddFixture()
{
    if (m_scene == NULL)
        return;

    /* Get a list of new fixtures to add to the scene */
    FixtureSelection fs(monitor(), doc());
    fs.setMultiSelection(true);
    if (fs.exec() == QDialog::Accepted)
    {
        QListIterator <quint32> it(fs.selection());
        while (it.hasNext() == true)
        {
            quint32 fid = it.next();
            m_scene->addFixture(doc(), fid);
            props()->setFixture3dProperties(fid, Fixture3dProperties());
            doc()->setModified();
        }
    }
}

void Monitor3dMode::slotRemoveFixture()
{
}

void Monitor3dMode::slotAddObject()
{
    if (m_scene == NULL)
        return;

    QString modelPath = QFileDialog::getOpenFileName(
        0,
        tr("Select model"),
        QLCFile::systemDirectory(MODELSDIR).path(),
        tr("3D models (*.osgt *.osgb *.3ds)"));

    if (modelPath.isEmpty())
        return; 

    m_scene->addObject(modelPath);
    doc()->setModified();
}

void Monitor3dMode::slotShowLabels(bool visible)
{
    props()->setLabelsVisible(visible);
}

