/*
  Q Light Controller Plus
  monitorgraphicsmode.cpp

  Copyright (c) Heikki Junnila

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
#include <QSplitter>
#include <QToolBar>
#include <QSpinBox>
#include <QAction>
#include <QLabel>
#include <QFont>
#include <QIcon>
#include <QtXml>

#include "monitorgraphicsmode.h"
#include "monitorfixturepropertieseditor.h"
#include "monitorbackgroundselection.h"
#include "monitorgraphicsview.h"
#include "fixtureselection.h"
#include "monitorfixture.h"
#include "monitorlayout.h"
#include "universe.h"
#include "monitor.h"
#include "apputil.h"
#include "doc.h"

#include "qlcfile.h"

#define SETTINGS_GEOMETRY "monitor/geometry"
#define SETTINGS_VSPLITTER "monitor/vsplitter"

/*****************************************************************************
 * Initialization
 *****************************************************************************/

MonitorGraphicsMode::MonitorGraphicsMode(QWidget* monitor, Doc* doc)
    : MonitorMode(monitor, doc)
{
}

MonitorGraphicsMode::~MonitorGraphicsMode()
{
}

QString MonitorGraphicsMode::name() const
{
    return tr("2D View");
}

MonitorProperties::DisplayMode MonitorGraphicsMode::displayMode() const
{
    return MonitorProperties::Graphics;
}

void MonitorGraphicsMode::connectSignals()
{
    /* Listen to fixture additions and changes from Doc */
    connect(doc(), SIGNAL(fixtureChanged(quint32)),
        this, SLOT(slotFixtureChanged(quint32)));
    connect(doc(), SIGNAL(fixtureRemoved(quint32)),
        this, SLOT(slotFixtureRemoved(quint32)));

    connect(doc()->inputOutputMap(), SIGNAL(universesWritten(int, const QByteArray&)),
        this, SLOT(slotUniversesWritten(int, const QByteArray&)));
}

void MonitorGraphicsMode::disconnectSignals()
{
    disconnect(doc(), SIGNAL(fixtureChanged(quint32)),
        this, SLOT(slotFixtureChanged(quint32)));
    disconnect(doc(), SIGNAL(fixtureRemoved(quint32)),
        this, SLOT(slotFixtureRemoved(quint32)));

    disconnect(doc()->inputOutputMap(), SIGNAL(universesWritten(int, const QByteArray&)),
        this, SLOT(slotUniversesWritten(int, const QByteArray&)));
}

void MonitorGraphicsMode::saveSettings()
{
    if (m_splitter != NULL)
    {
        QSettings settings;
        settings.setValue(SETTINGS_VSPLITTER, m_splitter->saveState());
    }
}

void MonitorGraphicsMode::initToolBar(QToolBar* toolBar)
{
    QLabel *label = new QLabel(tr("Size"));
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    toolBar->addWidget(label);

    QSize gridSize = props()->gridSize();

    m_gridWSpin = new QSpinBox();
    m_gridWSpin->setMinimum(1);
    m_gridWSpin->setValue(gridSize.width());
    toolBar->addWidget(m_gridWSpin);
    connect(m_gridWSpin, SIGNAL(valueChanged(int)),
        this, SLOT(slotGridWidthChanged(int)));

    QLabel *xlabel = new QLabel("x");
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    toolBar->addWidget(xlabel);
    m_gridHSpin = new QSpinBox();
    m_gridHSpin->setMinimum(1);
    m_gridHSpin->setValue(gridSize.height());
    toolBar->addWidget(m_gridHSpin);
    connect(m_gridHSpin, SIGNAL(valueChanged(int)),
        this, SLOT(slotGridHeightChanged(int)));

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

    toolBar->addAction(QIcon(":/image.png"), tr("Set a background picture"),
        this, SLOT(slotSetBackground()));

    m_labelsAction = toolBar->addAction(QIcon(":/label.png"), tr("Show/hide labels"));
    m_labelsAction->setCheckable(true);
    m_labelsAction->setChecked(props()->labelsVisible());
    connect(m_labelsAction, SIGNAL(triggered(bool)),
        this, SLOT(slotShowLabels(bool)));
}

void MonitorGraphicsMode::destroyToolBar()
{
    if (m_gridHSpin != NULL)
    {
        disconnect(m_gridWSpin, SIGNAL(valueChanged(int)),
            this, SLOT(slotGridWidthChanged(int)));
        m_gridWSpin->deleteLater();
        m_gridWSpin = NULL;
    }

    if (m_gridHSpin)
    {
        disconnect(m_gridHSpin, SIGNAL(valueChanged(int)),
            this, SLOT(slotGridHeightChanged(int)));
        m_gridHSpin->deleteLater();
        m_gridHSpin = NULL;
    }

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

void MonitorGraphicsMode::initUi()
{
    m_splitter = new QSplitter(Qt::Horizontal, monitor());
    monitor()->layout()->addWidget(m_splitter);
    QWidget* gcontainer = new QWidget(monitor());
    m_splitter->addWidget(gcontainer);
    gcontainer->setLayout(new QVBoxLayout);
    gcontainer->layout()->setContentsMargins(0, 0, 0, 0);

    m_graphicsView = new MonitorGraphicsView(doc(), monitor());
    m_graphicsView->setRenderHint(QPainter::Antialiasing);
    m_graphicsView->setAcceptDrops(true);
    m_graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_graphicsView->setBackgroundBrush(QBrush(QColor(11, 11, 11, 255), Qt::SolidPattern));
    m_splitter->widget(0)->layout()->addWidget(m_graphicsView);

    if (props()->gridUnits() == MonitorProperties::Meters)
        m_graphicsView->setGridMetrics(1000.0);
    else if (props()->gridUnits() == MonitorProperties::Feet)
        m_graphicsView->setGridMetrics(304.8);
    m_graphicsView->setGridSize(props()->gridSize());

    if (props()->commonBackgroundImage().isEmpty() == false)
        m_graphicsView->setBackgroundImage(props()->commonBackgroundImage());

    foreach (quint32 fid, props()->fixtureItemsID())
    {
        if (doc()->fixture(fid) != NULL)
        {
            m_graphicsView->addFixture(fid, props()->fixturePosition(fid));
            qDebug() << "Gel color:" << props()->fixtureGelColor(fid);
            m_graphicsView->setFixtureGelColor(fid, props()->fixtureGelColor(fid));
            m_graphicsView->setFixtureRotation(fid, props()->fixtureRotation(fid));
        }
    }

    setMonitorUniverse(Universe::invalid());

    m_graphicsView->showFixturesLabels(props()->labelsVisible());

    connect(m_graphicsView, SIGNAL(fixtureMoved(quint32,QPointF)),
            this, SLOT(slotFixtureMoved(quint32,QPointF)));
    connect(m_graphicsView, SIGNAL(viewClicked(QMouseEvent*)),
            this, SLOT(slotViewCliked()));

    // add container for chaser editor
    QWidget* econtainer = new QWidget(monitor());
    m_splitter->addWidget(econtainer);
    econtainer->setLayout(new QVBoxLayout);
    econtainer->layout()->setContentsMargins(0, 0, 0, 0);
    m_splitter->widget(1)->hide();

    QSettings settings;
    QVariant var2 = settings.value(SETTINGS_VSPLITTER);
    if (var2.isValid() == true)
        m_splitter->restoreState(var2.toByteArray());
}

void MonitorGraphicsMode::destroyUi()
{
    hideFixtureItemEditor();

    if (m_graphicsView)
    {
        disconnect(m_graphicsView, SIGNAL(fixtureMoved(quint32,QPointF)),
            this, SLOT(slotFixtureMoved(quint32,QPointF)));
        disconnect(m_graphicsView, SIGNAL(viewClicked(QMouseEvent*)),
            this, SLOT(slotViewCliked()));

        m_graphicsView->deleteLater();
        m_graphicsView = NULL;
    }

    if (m_splitter != NULL)
    {
        saveSettings();
        m_splitter->deleteLater();
        m_splitter = NULL;
    }
}

/****************************************************************************
 * Fixture added/removed stuff
 ****************************************************************************/

void MonitorGraphicsMode::slotFixtureChanged(quint32 fxi_id)
{
    if (m_graphicsView != NULL)
        m_graphicsView->updateFixture(fxi_id);
}

void MonitorGraphicsMode::slotFixtureRemoved(quint32 fxi_id)
{
     if (m_graphicsView != NULL)
         m_graphicsView->removeFixture(fxi_id);
}

void MonitorGraphicsMode::slotUniversesWritten(int index, const QByteArray& ua)
{
    if (m_graphicsView != NULL)
        m_graphicsView->writeUniverse(index, ua);
}

/********************************************************************
 * Graphics View
 ********************************************************************/

void MonitorGraphicsMode::slotGridWidthChanged(int value)
{
    if (m_graphicsView != NULL)
    {
        m_graphicsView->setGridSize(QSize(value, m_gridHSpin->value()));
        props()->setGridSize(QSize(value, m_gridHSpin->value()));
    }
}

void MonitorGraphicsMode::slotGridHeightChanged(int value)
{
    if (m_graphicsView != NULL)
    {
        m_graphicsView->setGridSize(QSize(m_gridWSpin->value(), value));
        props()->setGridSize(QSize(m_gridWSpin->value(), value));
    }
}

void MonitorGraphicsMode::slotGridUnitsChanged(int index)
{
    if (m_graphicsView != NULL)
    {
        MonitorProperties::GridUnits units = MonitorProperties::Meters;

        QVariant var = m_unitsCombo->itemData(index);
        if (var.isValid())
            units = MonitorProperties::GridUnits(var.toInt());

        if (units == MonitorProperties::Meters)
            m_graphicsView->setGridMetrics(1000.0);
        else if (units == MonitorProperties::Feet)
            m_graphicsView->setGridMetrics(304.8);

        props()->setGridUnits(units);
    }
}

void MonitorGraphicsMode::slotAddFixture()
{
    if (m_graphicsView == NULL)
        return;

    QList <quint32> disabled = m_graphicsView->fixturesID();
    /* Get a list of new fixtures to add to the scene */
    FixtureSelection fs(monitor(), doc());
    fs.setMultiSelection(true);
    fs.setDisabledFixtures(disabled);
    if (fs.exec() == QDialog::Accepted)
    {
        QListIterator <quint32> it(fs.selection());
        while (it.hasNext() == true)
        {
            quint32 fid = it.next();
            m_graphicsView->addFixture(fid);
            props()->setFixturePosition(fid, QPointF(0, 0));
            doc()->setModified();
        }
    }
    if (m_labelsAction->isChecked())
        slotShowLabels(true);
}

void MonitorGraphicsMode::slotRemoveFixture()
{
    if (m_graphicsView != NULL)
    {
        hideFixtureItemEditor();
        if (m_graphicsView->removeFixture() == true)
            doc()->setModified();
    }
}

void MonitorGraphicsMode::slotSetBackground()
{
    Q_ASSERT(m_graphicsView != NULL);

    MonitorBackgroundSelection mbgs(monitor(), doc());

    if (mbgs.exec() == QDialog::Accepted)
    {
        if (props()->commonBackgroundImage().isEmpty() == false)
            m_graphicsView->setBackgroundImage(props()->commonBackgroundImage());
        else
            m_graphicsView->setBackgroundImage(QString());

        doc()->setModified();
    }
}

void MonitorGraphicsMode::slotShowLabels(bool visible)
{
    if (m_graphicsView == NULL)
        return;

    props()->setLabelsVisible(visible);
    m_graphicsView->showFixturesLabels(visible);
}

void MonitorGraphicsMode::slotFixtureMoved(quint32 fid, QPointF pos)
{
    showFixtureItemEditor();
    props()->setFixturePosition(fid, pos);
    doc()->setModified();
}

void MonitorGraphicsMode::slotViewCliked()
{
    hideFixtureItemEditor();
}

void MonitorGraphicsMode::hideFixtureItemEditor()
{
    if (m_fixtureItemEditor != NULL)
    {
        m_splitter->widget(1)->layout()->removeWidget(m_fixtureItemEditor);
        m_splitter->widget(1)->hide();
        //m_fixtureItemEditor->deleteLater();
        m_fixtureItemEditor = NULL;
    }
}

void MonitorGraphicsMode::showFixtureItemEditor()
{
    MonitorFixtureItem *item = m_graphicsView->getSelectedItem();
    hideFixtureItemEditor();

    if (item != NULL)
    {

        m_fixtureItemEditor = new MonitorFixturePropertiesEditor(
                    item, m_graphicsView,
                    props(), m_splitter->widget(1));
        m_splitter->widget(1)->layout()->addWidget(m_fixtureItemEditor);
        m_splitter->widget(1)->show();
        m_fixtureItemEditor->show();
    }
}
