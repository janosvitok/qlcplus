/*
  Q Light Controller Plus
  monitor.h

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

#ifndef MONITOR_H
#define MONITOR_H

#include <QWidget>
#include <QMap>

#include "monitorproperties.h"

class Doc;
class MonitorMode;
class QToolBar;

/** \addtogroup ui_mon DMX Monitor
 * @{
 */

class Monitor : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Monitor)

    /*********************************************************************
     * Initialization
     *********************************************************************/
public:
    /** Get the monitor singleton instance. Can be NULL. */
    static Monitor* instance();

    /** Create or show Monitor */
    static void createAndShow(QWidget* parent, Doc* doc);

    /** Normal public destructor */
    ~Monitor();

protected:
    /** Protected constructor to prevent multiple instances. */
    Monitor(QWidget* parent, Doc* doc, Qt::WindowFlags f = 0);

    void saveSettings();
    void initToolBar();
    MonitorMode* currentMode();

    void start(MonitorMode* mode);
    void stop(MonitorMode* mode);

protected:
    /** The singleton Monitor instance */
    static Monitor* s_instance;
    Doc* m_doc;
    MonitorProperties *m_props;
    QMap<MonitorProperties::DisplayMode, MonitorMode*> m_modes;

    QToolBar* m_toolBar;

protected slots:

    /** Menu action slot to trigger display mode switch */
    void slotSwitchMode(int mode);
};

/** @} */

#endif
