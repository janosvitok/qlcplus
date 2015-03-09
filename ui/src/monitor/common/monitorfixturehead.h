/*
  Q Light Controller Plus
  monitorfixturehead.h

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

#ifndef MONITORFIXTUREHEAD_H
#define MONITORFIXTUREHEAD_H

class QByteArray;
class Fixture;

#include <QColor>

/** \addtogroup ui_mon DMX Monitor
 * @{
 */

class MonitorFixtureHead
{
public:
    MonitorFixtureHead(Fixture &fixture, int head);

    qreal computeTiltPosition(const QByteArray & ua);
    qreal computePanPosition(const QByteArray & ua);
    QColor computeColor(const QByteArray & ua);
    uchar computeAlpha(const QByteArray & ua);

    bool hasPan() const;
    bool hasTilt() const;
    bool hasMasterDimmer() const;

    int panRange() const { return m_panMaxDegrees; }
    int tiltRange() const { return m_tiltMaxDegrees; }

    void setGelColor(QColor color) { m_gelColor = color; }

private:
    //! cached rgb channels (absolute numbers)
    QList <quint32> m_rgb;

    //! cached cmy channels (absolute numbers)
    QList <quint32> m_cmy;

    //! cached color channels (absolute numbers)
    QList <quint32> m_colorWheels;

    //! map DMX values to colors
    /*! map channel -> array of 256 QColors
     */
    QHash<quint32, QList<QColor> > m_colorValues;

    /*! cached shutter channels (absolute values)
     */
    QList <quint32> m_shutterChannels;

    enum ShutterState
    {
        Closed,
        Strobe,
        Open
    };

    //! map DMX values to ON/OFF
    /*! map channel -> array of 256 bool values
     */
    QHash<quint32, QList<ShutterState> > m_shutterValues;

    quint32 m_masterDimmer;
    quint32 m_panChannel;
    int m_panMaxDegrees;
    quint32 m_tiltChannel;
    int m_tiltMaxDegrees;
    int m_strobePhase;

    QColor m_gelColor;
};

/** @} */

#endif // MONITORFIXTUREHEAD_H
