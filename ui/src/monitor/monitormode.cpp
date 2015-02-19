/*
  Q Light Controller Plus
  monitormode.cpp

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

#include "monitormode.h"
#include "doc.h"
#include "universe.h"

/*****************************************************************************
 * Initialization
 *****************************************************************************/

MonitorMode::MonitorMode(QWidget* monitor, Doc* doc)
    : m_doc(doc)
    , m_props(NULL)
    , m_monitor(monitor)
{
    Q_ASSERT(doc != NULL);

    m_props = m_doc->monitorProperties();
}

MonitorMode::~MonitorMode()
{
}

void MonitorMode::setMonitorUniverse(quint32 universe)
{
    for (quint32 i = 0; i < doc()->inputOutputMap()->universes(); i++)
    {
        quint32 uniID = doc()->inputOutputMap()->getUniverseID(i);
        if (universe == Universe::invalid() || uniID == universe)
            doc()->inputOutputMap()->setUniverseMonitor(i, true);
        else
            doc()->inputOutputMap()->setUniverseMonitor(i, false);
    }
}

Doc* MonitorMode::doc()
{
    return m_doc;
}

MonitorProperties* MonitorMode::props()
{
    return m_props;
}

QWidget* MonitorMode::monitor()
{
    return m_monitor;
}
