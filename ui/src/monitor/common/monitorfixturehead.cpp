/*
  Q Light Controller Plus
  monitorfixturehead.cpp

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

#include <qmath.h>
#include <QDebug>

#include "monitorfixturehead.h"
#include "qlcfixturehead.h"
#include "qlcfixturemode.h"
#include "qlcfixturedef.h"
#include "qlccapability.h"
#include "fixture.h"

#define STROBE_PERIOD 30

MonitorFixtureHead::MonitorFixtureHead(Fixture & fixture, int h)
    : m_masterDimmer(QLCChannel::invalid())
    , m_panChannel(QLCChannel::invalid())
    , m_panMaxDegrees(540)
    , m_tiltChannel(QLCChannel::invalid())
    , m_tiltMaxDegrees(270)
    , m_strobePhase(0)
{
    QLCFixtureHead head = fixture.head(h);
    QLCFixtureMode *mode = fixture.fixtureMode();

    if (head.masterIntensityChannel() != QLCChannel::invalid())
    {
        m_masterDimmer = fixture.address() + head.masterIntensityChannel();
        qDebug() << "Set master dimmer to:" << m_masterDimmer;
    }

    if (mode != NULL)
    {
        // color channels
        foreach (quint32 rgbComp, head.rgbChannels())
        {
            m_rgb.append(rgbComp + fixture.address());
            qDebug() << "Add RGB comp at address:" << rgbComp + fixture.address();
        }

        foreach (quint32 cmyComp, head.cmyChannels())
        {
            m_cmy.append(cmyComp + fixture.address());
            qDebug() << "Add CMY comp at address:" << cmyComp + fixture.address();
        }

        if (head.panMsbChannel() != QLCChannel::invalid())
        {
            m_panChannel = head.panMsbChannel() + fixture.address();
            // retrieve the PAN max degrees from the fixture mode
            if (mode->physical().focusPanMax() != 0)
                m_panMaxDegrees = mode->physical().focusPanMax();
            qDebug() << "Pan channel on" << m_panChannel << "max degrees:" << m_panMaxDegrees;
        }

        if (head.tiltMsbChannel() != QLCChannel::invalid())
        {
            m_tiltChannel = head.tiltMsbChannel() + fixture.address();
            // retrieve the TILT max degrees from the fixture mode
            if (mode->physical().focusTiltMax() != 0)
                m_tiltMaxDegrees = mode->physical().focusTiltMax();
            qDebug() << "Tilt channel on" << m_tiltChannel << "max degrees:" << m_tiltMaxDegrees;
        }

        foreach (quint32 wheel, head.colorWheels())
        {
            QList<QColor> values;
            QLCChannel *ch = mode->channel(wheel);
            if (ch == NULL)
                continue;
 
            bool containsColor = false;
            for(quint32 i = 0; i < 256; ++i)
            {
                QLCCapability *cap = ch->searchCapability(i);
                if (cap != NULL)
                {
                    values << cap->resourceColor1();
                    containsColor = true;
                }
                else
                {
                    values << QColor();
                }
            }

            if (containsColor)
            {
                m_colorValues[wheel + fixture.address()] = values;
                m_colorWheels << (wheel + fixture.address());
            }
        }

        foreach (quint32 shutter, head.shutterChannels())
        {
            QList<ShutterState> values;
            QLCChannel *ch = mode->channel(shutter);
            if (ch == NULL)
                continue;
 
            bool containsShutter = false;
            for(quint32 i = 0; i < 256; ++i)
            {
                QLCCapability *cap = ch->searchCapability(i);
                if (cap != NULL)
                {
                    if (cap->name().contains("close", Qt::CaseInsensitive) 
                        || cap->name().contains("blackout", Qt::CaseInsensitive))
                    {
                        values << Closed;
                        containsShutter = true;
                    }
                    else if (cap->name().contains("strob", Qt::CaseInsensitive) 
                        || cap->name().contains("pulse", Qt::CaseInsensitive))
                    {
                        values << Strobe;
                        containsShutter = true;
                    }
                    else
                        values << Open;
                }
                else
                {
                    values << Open;
                }
            }

            if (containsShutter)
            {
                m_shutterValues[shutter + fixture.address()] = values;
                m_shutterChannels << (shutter + fixture.address());
            }
        }
    }
}

QColor MonitorFixtureHead::computeColor(const QByteArray & ua)
{
    foreach (quint32 c, m_colorWheels)
    {
        const uchar val = (int(c) < ua.size()) ? static_cast<uchar>(ua.at(c)) : 0;
        QColor col = m_colorValues[c].at(val);
        if (col.isValid())
            return col;
    }

    if (m_rgb.count() > 0)
    {
        uchar r = 0, g = 0, b = 0;
        if (m_rgb.at(0) < (quint32)ua.count())
            r = ua.at(m_rgb.at(0));
        if (m_rgb.at(1) < (quint32)ua.count())
            g = ua.at(m_rgb.at(1));
        if (m_rgb.at(2) < (quint32)ua.count())
            b = ua.at(m_rgb.at(2));
        return QColor(r, g, b);
    }

    if (m_cmy.count() > 0)
    {
        uchar c = 0, m = 0, y = 0;
        if (m_cmy.at(0) < (quint32)ua.count())
            c = ua.at(m_cmy.at(0));
        if (m_cmy.at(1) < (quint32)ua.count())
            m = ua.at(m_cmy.at(1));
        if (m_cmy.at(2) < (quint32)ua.count())
            y = ua.at(m_cmy.at(2));
        return QColor::fromCmyk(c, m, y, 0);
    }
    
    if (m_gelColor.isValid())
    {
        return m_gelColor;
    }

    return QColor(255,255,255);
}
uchar MonitorFixtureHead::computeAlpha(const QByteArray & ua)
{
    uchar alpha = 255;
    if (m_masterDimmer != QLCChannel::invalid())
    {
        if (m_masterDimmer < (quint32)ua.size())
        {
            alpha = ua.at(m_masterDimmer);
        }
        else
        {
            alpha = 0; // incomplete universe is sent
        }
    }

    if (alpha == 0)
        return alpha; // once the shutter is closed, no light will come through, regardless if other wheels are open 

    foreach (quint32 c, m_shutterChannels)
    {
        const uchar val = (int(c) < ua.size()) ? static_cast<uchar>(ua.at(c)) : 0;
        ShutterState state = m_shutterValues[c].at(val);
        if (state == Closed) 
        {
            alpha = 0;
            m_strobePhase = -1;
            return alpha;
        }
        else if (state == Strobe)
        {
            if (m_strobePhase == -1)
                m_strobePhase = 0;
            if (m_strobePhase < STROBE_PERIOD/2)
                alpha = 0;
            ++m_strobePhase;
            if (m_strobePhase > STROBE_PERIOD)
                m_strobePhase = 0;
            if (alpha == 0)
                return alpha;
        }
        else
        {
            m_strobePhase = -1;
        }
    }

    return alpha;
}

qreal MonitorFixtureHead::computeTiltPosition(const QByteArray & ua)
{
    qreal value = 0.0;
    if (!hasTilt())
        return value;

    if (m_tiltChannel < (quint32)ua.size())
    {
        value = ((uchar)ua.at(m_tiltChannel) * m_tiltMaxDegrees) / 255.0 - (m_tiltMaxDegrees / 2.0);
    }
    else
    {
        value = -(m_tiltMaxDegrees / 2.0);
    }

    // qDebug() << "TILT degrees:" << value;
    return value;
}

qreal MonitorFixtureHead::computePanPosition(const QByteArray & ua)
{
    qreal value = 0.0;
    if (!hasPan())
        return value;

    if (m_panChannel < (quint32)ua.size())
    {
        value = ((uchar)ua.at(m_panChannel) * m_panMaxDegrees) / 255.0 - (m_panMaxDegrees / 2.0);
    }
    else
    {
        value = -(m_panMaxDegrees / 2.0);
    }

    // qDebug() << "PAN degrees:" << value;
    return value;
}

bool MonitorFixtureHead::hasPan() const
{
    return m_panChannel != QLCChannel::invalid();
}

bool MonitorFixtureHead::hasTilt() const
{
    return m_tiltChannel != QLCChannel::invalid();
}

bool MonitorFixtureHead::hasMasterDimmer() const
{
    return m_masterDimmer != QLCChannel::invalid();
}
