/*
  Q Light Controller Plus
  monitorfixturebase.h

  Copyright (C) Massimo Callegari

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

#ifndef MONITORFIXTUREBASE_H
#define MONITORFIXTUREBASE_H

#include "monitorfixturehead.h"
#include <QColor>

class Doc;

class MonitorFixtureHead;
class Fixture;

/** \addtogroup ui_mon DMX Monitor
 * @{
 */

class MonitorFixtureBase : public QObject
{
    Q_OBJECT

public:
    MonitorFixtureBase(Doc *doc, quint32 fid);

    virtual ~MonitorFixtureBase();

    void initialize();

    /** Get the fixture name as displayed on the label */
    QString name() const { return m_name; }

    void setGelColor(QColor color);
    QColor getColor() { return m_gelColor; }

    /** Return the fixture ID associated to this item */
    quint32 fixtureID() { return m_fid; }

    /** Return the number of heads represented by this item */
    int headsCount() { return m_heads.count(); }

protected:
    virtual MonitorFixtureHead* createHead(Fixture & fixture, int head) = 0;

protected:
    QList <MonitorFixtureHead *> m_heads;

private:
    Doc *m_doc;

    /** The Fixture ID this item is associated to */
    quint32 m_fid;

    /** The fixture name */
    QString m_name;


    /** In case of a dimmer, this hold the gel color to apply */
    QColor m_gelColor;
};

/** @} */

#endif // MONITORFIXTUREBASE_H
