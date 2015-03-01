/*
  Q Light Controller Plus
  monitordmxmode.h

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

#ifndef MONITORDMXMODE_H
#define MONITORDMXMODE_H

#include "monitormode.h"
#include "monitorproperties.h"

class QToolBar;
class QScrollArea;
class MonitorLayout;
class MonitorFixture;
class Fixture;
class QByteArray;

/** \addtogroup ui_mon_dmx DMX View
 * @{
 */

class MonitorDmxMode : public MonitorMode
{
    Q_OBJECT
    Q_DISABLE_COPY(MonitorDmxMode)

    /*********************************************************************
     * Initialization
     *********************************************************************/
public:

    MonitorDmxMode(QWidget* parent, Doc* doc);

    ~MonitorDmxMode();

    QString name() const;
    MonitorProperties::DisplayMode displayMode() const;

    void connectSignals();
    void disconnectSignals();

    void saveSettings();
    void initToolBar(QToolBar * toolBar);
    void destroyToolBar();
    void initUi();
    void destroyUi();

protected slots:
    /** Menu action slot for font selection */
    void slotChooseFont();

    /** Menu action slot for channel style selection */
    void slotChannelStyleTriggered();

    /** Menu action slot for value style selection */
    void slotValueStyleTriggered();

    /********************************************************************
     * Monitor Fixtures
     ********************************************************************/
public:
    /** Update monitor fixture labels */
    void updateFixtureLabelStyles();

protected:
    /** Create a new MonitorFixture* and append it to the layout */
    void createMonitorFixture(Fixture* fxi);

protected slots:
    /** Slot for fixture additions (to append the new fixture to layout) */
    void slotFixtureAdded(quint32 fxi_id);

    /** Slot for fixture contents & layout changes */
    void slotFixtureChanged(quint32 fxi_id);

    /** Slot for fixture removals (to remove the fixture from layout) */
    void slotFixtureRemoved(quint32 fxi_id);

    /** Slot called when a universe combo item is selected */
    void slotUniverseSelected(int index);

    /** Slot for getting the latest values from InputOutputMap */
    void slotUniversesWritten(int index, const QByteArray& ua);

signals:
    void channelStyleChanged(MonitorProperties::ChannelStyle style);
    void valueStyleChanged(MonitorProperties::ValueStyle style);

protected:
    QScrollArea* m_scrollArea;
    QWidget* m_monitorWidget;
    MonitorLayout* m_monitorLayout;
    QList <MonitorFixture*> m_monitorFixtures;
    quint32 m_currentUniverse;
};

/** @} */

#endif
