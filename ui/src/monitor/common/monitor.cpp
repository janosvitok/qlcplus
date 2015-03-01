/*
  Q Light Controller Plus
  monitor.cpp

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

#include <QAction>
#include <QVBoxLayout>
#include <QSettings>
#include <QToolBar>
#include <QComboBox>

#include "monitor.h"
#include "apputil.h"
#include "doc.h"

#include "monitordmxmode.h"
#include "monitorgraphicsmode.h"
#include "monitor3dmode.h"

#define SETTINGS_GEOMETRY "monitor/geometry"

Monitor* Monitor::s_instance = NULL;

/*****************************************************************************
 * Initialization
 *****************************************************************************/

Monitor::Monitor(QWidget* parent, Doc* doc, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_doc(doc)
    , m_props(NULL)
{
    Q_ASSERT(doc != NULL);

    m_props = m_doc->monitorProperties();

    /* Master layout for toolbar and scroll area */
    new QVBoxLayout(this);

    m_modes.insert(MonitorProperties::DMX, new MonitorDmxMode(this, doc));
    m_modes.insert(MonitorProperties::Graphics, new MonitorGraphicsMode(this, doc));
    m_modes.insert(MonitorProperties::ThreeD, new Monitor3dMode(this, doc));

    start(currentMode());
}

Monitor::~Monitor()
{
    stop(currentMode());

    foreach(MonitorProperties::DisplayMode key, m_modes.keys())
        delete m_modes.take(key);

    saveSettings();

    /* Reset the singleton instance */
    Monitor::s_instance = NULL;
}

Monitor* Monitor::instance()
{
    return s_instance;
}

void Monitor::saveSettings()
{
    QSettings settings;
    settings.setValue(SETTINGS_GEOMETRY, saveGeometry());

    foreach(MonitorMode* mode, m_modes)
    {
        if (mode != NULL)
            mode->saveSettings();
    }
}

void Monitor::createAndShow(QWidget* parent, Doc* doc)
{
    QWidget* window = NULL;

    /* Must not create more than one instance */
    if (s_instance == NULL)
    {
        /* Create a separate window for OSX */
        s_instance = new Monitor(parent, doc, Qt::Window);
        window = s_instance;

        /* Set some common properties for the window and show it */
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWindowIcon(QIcon(":/monitor.png"));
        window->setWindowTitle(tr("Fixture Monitor"));
        window->setContextMenuPolicy(Qt::CustomContextMenu);

        QSettings settings;
        QVariant var = settings.value(SETTINGS_GEOMETRY);
        if (var.isValid() == true)
            window->restoreGeometry(var.toByteArray());
        else
        {
            window->resize(800, 600);
            window->move(50, 50);
        }
        AppUtil::ensureWidgetIsVisible(window);
    }
    else
    {
        window = s_instance;
    }

    window->show();
    window->raise();
}

void Monitor::start(MonitorMode* mode)
{
    initToolBar();

    mode->initUi();
    mode->initToolBar(m_toolBar);
    mode->connectSignals();
}

void Monitor::stop(MonitorMode* mode)
{
    mode->disconnectSignals();
    mode->destroyUi();

    mode->destroyToolBar();
    m_toolBar->deleteLater();
    m_toolBar = NULL;
}

void Monitor::initToolBar()
{
    m_toolBar = new QToolBar(this);

    /* Menu bar */
    Q_ASSERT(layout() != NULL);
    layout()->setMenuBar(m_toolBar);

    QComboBox *uniCombo = new QComboBox(this);
    foreach(MonitorMode* mode, m_modes)
    {
        uniCombo->addItem(mode->name(), mode->displayMode());
        if (mode == currentMode())
            uniCombo->setCurrentIndex(uniCombo->count()-1);
    }
    connect(uniCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(slotSwitchMode(int)));
    m_toolBar->addWidget(uniCombo);
    m_toolBar->addSeparator();
}

MonitorMode* Monitor::currentMode()
{
    return m_modes[m_props->displayMode()];
}

void Monitor::slotSwitchMode(int mode)
{
    stop(currentMode());
    m_props->setDisplayMode(MonitorProperties::DisplayMode(mode));
    start(currentMode());
}

