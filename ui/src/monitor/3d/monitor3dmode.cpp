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
#include <QLabel>
#include <QFont>
#include <QIcon>
#include <QtXml>

#include <osg/Geode>

#include "fixture.h"
#include "fixtureselection.h"
#include "universe.h"
#include "monitor.h"
#include "apputil.h"
#include "doc.h"
#include "qlcfile.h"

#include "monitor3dmode.h"
#include "osgwidget.h"
#include "myscene.h"

/*****************************************************************************
 * Initialization
 *****************************************************************************/

Monitor3dMode::Monitor3dMode(QWidget* monitor, Doc* doc)
    : MonitorMode(monitor, doc)
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

    toolBar->addAction(QIcon(":/wizard.png"), tr("Demo"),
        this, SLOT(slotDemoButtonPushed()));
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
    m_scene = new MyScene();
    m_osgWidget = new OSGWidget(m_scene, monitor());
    monitor()->layout()->addWidget(m_osgWidget);

    setMonitorUniverse(Universe::invalid());
}

void Monitor3dMode::destroyUi()
{
    m_scene = NULL;
    m_osgWidget->deleteLater();
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
    Q_UNUSED(fxi_id);
}

void Monitor3dMode::slotUniversesWritten(int index, const QByteArray& ua)
{
    if (m_scene == NULL)
        return;

    if (index != 0)
        return;
    
    m_scene->update(ua);
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
            m_scene->addFixture(fid);
            doc()->setModified();
        }
    }
}

void Monitor3dMode::slotRemoveFixture()
{
}

void Monitor3dMode::slotShowLabels(bool visible)
{
    props()->setLabelsVisible(visible);
}

void Monitor3dMode::slotDemoButtonPushed()
{
    if (m_scene == NULL)
        return;

    const int numberOfFixtures = 15;
    const float stageWidth = 15.0;
    const float space = stageWidth / (float)(numberOfFixtures + 1);
    float posX = stageWidth * -0.5f;
    for (int i = 0; i < numberOfFixtures; ++i)
    {
        m_scene->addFixture(i);
        posX += space;
        m_scene->getFixtures().last().changeColor(osg::Vec3((float)(i + 1)/(float)numberOfFixtures, 1.0f, 0.0f));
        m_scene->getFixtures().last().moveHead(posX, 4.0f + (float)i / 4, 4.0f);
        m_scene->getFixtures().last().changeOpacity(0.6);
    }
}
