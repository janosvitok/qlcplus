/*
  Q Light Controller Plus
  monitor3dmode.h

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

#ifndef MONITOR3DMODE_H
#define MONITOR3DMODE_H

#include "monitormode.h"

class QAction;
class QComboBox;
class QToolBar;
class QWidget;
class QSplitter;

class Doc;
class Fixture;

class MyScene;
class OSGWidget;

/** \addtogroup ui_mon_3d 3D View
 * @{
 */

class Monitor3dMode : public MonitorMode
{
    Q_OBJECT
    Q_DISABLE_COPY(Monitor3dMode)

    /*********************************************************************
     * Initialization
     *********************************************************************/
public:

    Monitor3dMode(QWidget* monitor, Doc* doc);

    ~Monitor3dMode();

    QString name() const;
    MonitorProperties::DisplayMode displayMode() const;

    void connectSignals();
    void disconnectSignals();

    void saveSettings();
    void initToolBar(QToolBar * toolBar);
    void destroyToolBar();
    void initUi();
    void destroyUi();

protected:

    void updateFixtureLabelStyles();
    void createMonitorFixture(Fixture* fxi);

protected slots:

    /** Slot for fixture contents & layout changes */
    void slotFixtureChanged(quint32 fxi_id);

    /** Slot for fixture removals (to remove the fixture from layout) */
    void slotFixtureRemoved(quint32 fxi_id);

    /** Slot for getting the latest values from InputOutputMap */
    void slotUniversesWritten(int index, const QByteArray& ua);

protected slots:

    /** Slot called when the unit metrics changes */
    void slotGridUnitsChanged(int index);

    /** Slot called when the user wants to add
     *  a fixture to the graphics view */
    void slotAddFixture();

    /** Slot called when the user wants to remove
     *  a fixture from the graphics view */
    void slotRemoveFixture();

    /** Slot called when the user wants to show
     *  or hide fixtures labels */
    void slotShowLabels(bool visible);

    void slotDemoButtonPushed();

protected:
    QSplitter* m_splitter;
    QWidget *m_fixtureItemEditor;
    QComboBox *m_unitsCombo;
    QAction *m_labelsAction;

    OSGWidget *m_osgWidget;
    MyScene *m_scene;
};

/** @} */

#endif
