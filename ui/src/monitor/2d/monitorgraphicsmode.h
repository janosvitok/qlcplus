/*
  Q Light Controller Plus
  monitorgraphicsmode.h

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

#ifndef MONITORGRAPHICSMODE_H
#define MONITORGRAPHICSMODE_H

#include <QPointF>
#include "monitormode.h"

class QAction;
class QComboBox;
class QSpinBox;
class QSplitter;
class QToolBar;

class Doc;
class Fixture;
class MonitorGraphicsView;

/** \addtogroup ui_mon_2d 2D View
 * @{
 */

class MonitorGraphicsMode : public MonitorMode
{
    Q_OBJECT
    Q_DISABLE_COPY(MonitorGraphicsMode)

    /*********************************************************************
     * Initialization
     *********************************************************************/
public:

    MonitorGraphicsMode(QWidget* monitor, Doc* doc);

    ~MonitorGraphicsMode();

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

protected:
    /** Hides the Fixture Item editor on the right side of the view */
    void hideFixtureItemEditor();

    /** Shows the Fixture Item editor on the right side of the view */
    void showFixtureItemEditor();

protected slots:
    /** Slot called when the grid width changes */
    void slotGridWidthChanged(int value);

    /** Slot called when the grid height changes */
    void slotGridHeightChanged(int value);

    /** Slot called when the unit metrics changes */
    void slotGridUnitsChanged(int index);

    /** Slot called when the user wants to add
     *  a fixture to the graphics view */
    void slotAddFixture();

    /** Slot called when the user wants to remove
     *  a fixture from the graphics view */
    void slotRemoveFixture();

    /** Slot called when the user wants to set
     *  a background picture on the graphics view */
    void slotSetBackground();

    /** Slot called when the user wants to show
     *  or hide fixtures labels */
    void slotShowLabels(bool visible);

    /** Slot called when a fixture is moved in the graphics view */
    void slotFixtureMoved(quint32 fid, QPointF pos);

    /** Slot called when the graphics view is clicked */
    void slotViewClicked();

protected:
    QSplitter* m_splitter;
    MonitorGraphicsView* m_graphicsView;
    QWidget *m_fixtureItemEditor;
    QSpinBox* m_gridWSpin;
    QSpinBox *m_gridHSpin;
    QComboBox *m_unitsCombo;
    QAction *m_labelsAction;
};

/** @} */

#endif
