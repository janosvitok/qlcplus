/*
  Q Light Controller Plus
  monitordmxmode.cpp

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

#include <QScrollArea>
#include <QWidget>
#include <QToolBar>
#include <QActionGroup>
#include <QLabel>
#include <QComboBox>
#include <QFontDialog>

#include "monitordmxmode.h"
#include "monitorfixture.h"
#include "monitorlayout.h"
#include "universe.h"
#include "doc.h"

#include "qlcfile.h"

#define SETTINGS_GEOMETRY "monitor/geometry"
#define SETTINGS_VSPLITTER "monitor/vsplitter"

/*****************************************************************************
 * Initialization
 *****************************************************************************/

MonitorDmxMode::MonitorDmxMode(QWidget* parent, Doc* d)
    : MonitorMode(parent, d)
    , m_scrollArea(NULL)
    , m_monitorWidget(NULL)
    , m_monitorLayout(NULL)
    , m_currentUniverse(Universe::invalid())
{
}

MonitorDmxMode::~MonitorDmxMode()
{
    destroyUi();
}

QString MonitorDmxMode::name() const
{
    return tr("DMX View");
}

MonitorProperties::DisplayMode MonitorDmxMode::displayMode() const
{
    return MonitorProperties::DMX;
}

void MonitorDmxMode::connectSignals()
{
    /* Listen to fixture additions and changes from Doc */
    connect(doc(), SIGNAL(fixtureAdded(quint32)),
            this, SLOT(slotFixtureAdded(quint32)));
    connect(doc(), SIGNAL(fixtureChanged(quint32)),
            this, SLOT(slotFixtureChanged(quint32)));
    connect(doc(), SIGNAL(fixtureRemoved(quint32)),
            this, SLOT(slotFixtureRemoved(quint32)));

    connect(doc()->inputOutputMap(), SIGNAL(universesWritten(int, const QByteArray&)),
            this, SLOT(slotUniversesWritten(int, const QByteArray&)));
}

void MonitorDmxMode::disconnectSignals()
{
    disconnect(doc(), SIGNAL(fixtureAdded(quint32)),
            this, SLOT(slotFixtureAdded(quint32)));
    disconnect(doc(), SIGNAL(fixtureChanged(quint32)),
            this, SLOT(slotFixtureChanged(quint32)));
    disconnect(doc(), SIGNAL(fixtureRemoved(quint32)),
            this, SLOT(slotFixtureRemoved(quint32)));

    disconnect(doc()->inputOutputMap(), SIGNAL(universesWritten(int, const QByteArray&)),
               this, SLOT(slotUniversesWritten(int, const QByteArray&)));
}

void MonitorDmxMode::saveSettings()
{
    if (m_monitorWidget != NULL)
        props()->setFont(m_monitorWidget->font());
}

void MonitorDmxMode::initToolBar(QToolBar* toolBar)
{
    QActionGroup* group;
    QAction* action;

    /* Font */
    toolBar->addAction(QIcon(":/fonts.png"), tr("Font"),
                       this, SLOT(slotChooseFont()));

    toolBar->addSeparator();

    /* Channel style */
    group = new QActionGroup(this);
    group->setExclusive(true);

    action = toolBar->addAction(tr("DMX Channels"));
    action->setToolTip(tr("Show absolute DMX channel numbers"));
    action->setCheckable(true);
    action->setData(MonitorProperties::DMXChannels);
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotChannelStyleTriggered()));
    toolBar->addAction(action);
    group->addAction(action);
    if (props()->channelStyle() == MonitorProperties::DMXChannels)
        action->setChecked(true);

    action = toolBar->addAction(tr("Relative Channels"));
    action->setToolTip(tr("Show channel numbers relative to fixture"));
    action->setCheckable(true);
    action->setData(MonitorProperties::RelativeChannels);
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotChannelStyleTriggered()));
    toolBar->addAction(action);
    group->addAction(action);
    if (props()->channelStyle() == MonitorProperties::RelativeChannels)
        action->setChecked(true);

    toolBar->addSeparator();

    /* Value display style */
    group = new QActionGroup(this);
    group->setExclusive(true);

    action = toolBar->addAction(tr("DMX Values"));
    action->setToolTip(tr("Show DMX values 0-255"));
    action->setCheckable(true);
    action->setData(MonitorProperties::DMXValues);
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotValueStyleTriggered()));
    toolBar->addAction(action);
    group->addAction(action);
    action->setChecked(true);
    if (props()->valueStyle() == MonitorProperties::DMXValues)
        action->setChecked(true);

    action = toolBar->addAction(tr("Percent Values"));
    action->setToolTip(tr("Show percentage values 0-100%"));
    action->setCheckable(true);
    action->setData(MonitorProperties::PercentageValues);
    connect(action, SIGNAL(triggered(bool)),
            this, SLOT(slotValueStyleTriggered()));
    toolBar->addAction(action);
    group->addAction(action);
    if (props()->valueStyle() == MonitorProperties::PercentageValues)
        action->setChecked(true);

    /* Universe combo box */
    toolBar->addSeparator();

    QLabel *uniLabel = new QLabel(tr("Universe"));
    uniLabel->setMargin(5);
    toolBar->addWidget(uniLabel);

    QComboBox *uniCombo = new QComboBox(monitor());
    uniCombo->addItem(tr("All universes"), Universe::invalid());
    for (quint32 i = 0; i < doc()->inputOutputMap()->universes(); i++)
    {
        quint32 uniID = doc()->inputOutputMap()->getUniverseID(i);
        uniCombo->addItem(doc()->inputOutputMap()->getUniverseNameByIndex(i), uniID);
    }
    connect(uniCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotUniverseSelected(int)));
    toolBar->addWidget(uniCombo);
}

void MonitorDmxMode::destroyToolBar()
{
}

void MonitorDmxMode::initUi()
{
    /* Scroll area that contains the monitor widget */
    m_scrollArea = new QScrollArea(monitor());
    m_scrollArea->setWidgetResizable(true);
    monitor()->layout()->addWidget(m_scrollArea);

    /* Monitor widget that contains all MonitorFixtures */
    m_monitorWidget = new QWidget(m_scrollArea);
    m_monitorWidget->setBackgroundRole(QPalette::Dark);
    m_monitorLayout = new MonitorLayout(m_monitorWidget);
    m_monitorLayout->setSpacing(1);
    m_monitorLayout->setMargin(1);
    m_monitorWidget->setFont(props()->font());

    /* Create a bunch of MonitorFixtures for each fixture */
    foreach(Fixture* fxi, doc()->fixtures())
    {
        Q_ASSERT(fxi != NULL);
        if (m_currentUniverse == Universe::invalid() ||
            m_currentUniverse == fxi->universe())
                createMonitorFixture(fxi);
    }

    setMonitorUniverse(m_currentUniverse);
    /* Show the master container widgets */
    m_scrollArea->setWidget(m_monitorWidget);
    m_monitorWidget->show();
    m_scrollArea->show();
}

void MonitorDmxMode::destroyUi()
{
    while (m_monitorFixtures.isEmpty() == false)
        delete m_monitorFixtures.takeFirst();

    monitor()->layout()->removeWidget(m_scrollArea);
    delete m_monitorWidget;
    m_monitorWidget = NULL;
    delete m_scrollArea;
    m_scrollArea = NULL;
}

void MonitorDmxMode::slotChooseFont()
{
    bool ok = false;
    QFont f = QFontDialog::getFont(&ok, m_monitorWidget->font(), monitor());
    if (ok == true)
    {
        m_monitorWidget->setFont(f);
        props()->setFont(f);
    }
}

void MonitorDmxMode::slotChannelStyleTriggered()
{
    QAction* action = qobject_cast<QAction*> (QObject::sender());
    Q_ASSERT(action != NULL);

    action->setChecked(true);
    props()->setChannelStyle(MonitorProperties::ChannelStyle(action->data().toInt()));
    emit channelStyleChanged(props()->channelStyle());
}

void MonitorDmxMode::slotValueStyleTriggered()
{
    QAction* action = qobject_cast<QAction*> (QObject::sender());
    Q_ASSERT(action != NULL);

    action->setChecked(true);
    props()->setValueStyle(MonitorProperties::ValueStyle(action->data().toInt()));
    emit valueStyleChanged(props()->valueStyle());
}

/****************************************************************************
 * Fixture added/removed stuff
 ****************************************************************************/

void MonitorDmxMode::createMonitorFixture(Fixture* fxi)
{
    MonitorFixture* mof = new MonitorFixture(m_monitorWidget, doc());
    mof->setFixture(fxi->id());
    mof->slotChannelStyleChanged(props()->channelStyle());
    mof->slotValueStyleChanged(props()->valueStyle());
    mof->show();

    /* Make mof listen to value & channel style changes */
    connect(this, SIGNAL(valueStyleChanged(MonitorProperties::ValueStyle)),
            mof, SLOT(slotValueStyleChanged(MonitorProperties::ValueStyle)));
    connect(this, SIGNAL(channelStyleChanged(MonitorProperties::ChannelStyle)),
            mof, SLOT(slotChannelStyleChanged(MonitorProperties::ChannelStyle)));

    m_monitorLayout->addItem(new MonitorLayoutItem(mof));
    m_monitorFixtures.append(mof);
}

void MonitorDmxMode::slotFixtureAdded(quint32 fxi_id)
{
    Fixture* fxi = doc()->fixture(fxi_id);
    if (fxi != NULL)
        createMonitorFixture(fxi);
}

void MonitorDmxMode::slotFixtureChanged(quint32 fxi_id)
{
    QListIterator <MonitorFixture*> it(m_monitorFixtures);
    while (it.hasNext() == true)
    {
        MonitorFixture* mof = it.next();
        if (mof->fixture() == fxi_id)
            mof->setFixture(fxi_id);
    }

    m_monitorLayout->sort();
    m_monitorWidget->updateGeometry();
}

void MonitorDmxMode::slotFixtureRemoved(quint32 fxi_id)
{
    QMutableListIterator <MonitorFixture*> it(m_monitorFixtures);
    while (it.hasNext() == true)
    {
        MonitorFixture* mof = it.next();
        if (mof->fixture() == fxi_id)
        {
            it.remove();
            delete mof;
        }
    }
}

void MonitorDmxMode::slotUniverseSelected(int index)
{
    QComboBox *combo = (QComboBox *)sender();
    m_currentUniverse = combo->itemData(index).toUInt();

    destroyUi();
    initUi();
}

void MonitorDmxMode::slotUniversesWritten(int index, const QByteArray& ua)
{
    QListIterator <MonitorFixture*> it(m_monitorFixtures);
    while (it.hasNext() == true)
        it.next()->updateValues(index, ua);
}
