/*
  Q Light Controller Plus
  monitormode.h

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

#ifndef MONITORMODE_H
#define MONITORMODE_H

#include <QObject>

#include "monitorproperties.h"

class QToolBar;
class QWidget;

class Doc;

/** \addtogroup ui_mon DMX Monitor
 * @{
 */

class MonitorMode : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MonitorMode)

    /*********************************************************************
     * Initialization
     *********************************************************************/
public:

    MonitorMode(QWidget* monitor, Doc* doc);

    ~MonitorMode();

    virtual QString name() const = 0;
    virtual MonitorProperties::DisplayMode displayMode() const = 0;

    virtual void connectSignals() = 0;
    virtual void disconnectSignals() = 0;

    virtual void saveSettings() = 0;
    virtual void initToolBar(QToolBar * toolBar) = 0;
    virtual void destroyToolBar() = 0;
    virtual void initUi() = 0;
    virtual void destroyUi() = 0;

protected:
    /** enable or disable universe monitors
     *  \param universe universeId - this universe are enabled, all others disabled
     *  \param universe Universe::invalid() - all universes enabled
     */
    void setMonitorUniverse(quint32 universe);

    Doc* doc();
    MonitorProperties* props();
    QWidget* monitor();

private:
    Doc* m_doc;
    MonitorProperties* m_props;
    QWidget* m_monitor;
};

/** @} */

#endif
