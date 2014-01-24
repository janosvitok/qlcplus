/*
  Q Light Controller Plus
  qlcfixturehead.cpp

  Copyright (C) Heikki Junnila
                Massimo Callegari

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

#include <QXmlStreamReader>
#include <QDebug>

#include "qlcfixturehead.h"
#include "qlcfixturemode.h"
#include "qlcchannel.h"

QLCFixtureHead::QLCFixtureHead()
    : m_channelsCached(false)
{
}

QLCFixtureHead::QLCFixtureHead(const QLCFixtureHead& head)
    : m_channels(head.m_channels)
    , m_channelsCached(head.m_channelsCached)
    , m_channelsMap(head.m_channelsMap)
    , m_colorWheels(head.m_colorWheels)
    , m_shutterChannels(head.m_shutterChannels)
{
}

QLCFixtureHead::~QLCFixtureHead()
{
}

/****************************************************************************
 * Channels
 ****************************************************************************/

void QLCFixtureHead::addChannel(quint32 channel)
{
    if (m_channels.contains(channel) == false)
        m_channels.append(channel);
}

void QLCFixtureHead::removeChannel(quint32 channel)
{
    m_channels.removeAll(channel);
}

QList<quint32> QLCFixtureHead::channels() const
{
    return m_channels;
}

/****************************************************************************
 * Cached channels
 ****************************************************************************/

quint32 QLCFixtureHead::channelNumber(int type) const
{
    return m_channelsMap.value(type, 0xFFFFFFFF);
}

QVector <quint32> QLCFixtureHead::rgbChannels() const
{
    QVector <quint32> vector;
    quint32 r = channelNumber(QLCChannel::Red);
    quint32 g = channelNumber(QLCChannel::Green);
    quint32 b = channelNumber(QLCChannel::Blue);

    if (r != QLCChannel::invalid() && g != QLCChannel::invalid() && b != QLCChannel::invalid())
        vector << r << g << b;

    return vector;
}

QVector <quint32> QLCFixtureHead::cmyChannels() const
{
    QVector <quint32> vector;
    quint32 c = channelNumber(QLCChannel::Cyan);
    quint32 m = channelNumber(QLCChannel::Magenta);
    quint32 y = channelNumber(QLCChannel::Yellow);

    if (c != QLCChannel::invalid() && m != QLCChannel::invalid() && y != QLCChannel::invalid())
        vector << c << m << y;

    return vector;
}

QVector <quint32> QLCFixtureHead::colorWheels() const
{
    return m_colorWheels;
}

QVector <quint32> QLCFixtureHead::shutterChannels() const
{
    return m_shutterChannels;
}

void QLCFixtureHead::setMapIndex(int chType, quint32 index)
{
    m_channelsMap[chType] = index;
}

void QLCFixtureHead::cacheChannels(const QLCFixtureMode* mode)
{
    Q_ASSERT(mode != NULL);

    // Allow only one caching round per fixture mode instance
    if (m_channelsCached == true)
        return;

    m_colorWheels.clear();
    m_shutterChannels.clear();
    m_channelsMap.clear();

    foreach(quint32 i, m_channels)
    {
        if ((int)i >= mode->channels().size())
        {
            qDebug() << "Head contains undefined channel" << i;
            continue;
        }

        const QLCChannel* ch = mode->channels().at(i);
        Q_ASSERT(ch != NULL);

        if (ch->controlByte() == QLCChannel::LSB)
            continue;

        if (ch->group() == QLCChannel::Pan)
        {
            setMapIndex(QLCChannel::Pan, i);
        }
        else if (ch->group() == QLCChannel::Tilt)
        {
            setMapIndex(QLCChannel::Tilt, i);
        }
        else if (ch->group() == QLCChannel::Intensity)
        {
            if (ch->colour() == QLCChannel::NoColour)
            {
                 setMapIndex(QLCChannel::Intensity, i);
            }
            else // all the other colors
            {
                setMapIndex(ch->colour(), i);
            }
        }
        else if (ch->group() == QLCChannel::Colour)
        {
            m_colorWheels << i;
        }
        else if (ch->group() == QLCChannel::Shutter)
        {
            m_shutterChannels << i;
        }
    }

    // if this head doesn't include any Pan/Tilt channel
    // try to retrieve them from the fixture Mode
    if (channelNumber(QLCChannel::Pan) == QLCChannel::invalid())
        setMapIndex(QLCChannel::Pan, mode->channelNumber(QLCChannel::Pan));
    if (channelNumber(QLCChannel::Tilt) == QLCChannel::invalid())
        setMapIndex(QLCChannel::Tilt, mode->channelNumber(QLCChannel::Tilt));

    qSort(m_colorWheels);
    qSort(m_shutterChannels);

    // Allow only one caching round per head
    m_channelsCached = true;
}

/****************************************************************************
 * Load & Save
 ****************************************************************************/

bool QLCFixtureHead::loadXML(QXmlStreamReader &doc)
{
    if (doc.name() != KXMLQLCFixtureHead)
    {
        qWarning() << Q_FUNC_INFO << "Fixture Head node not found!";
        return false;
    }

    while (doc.readNextStartElement())
    {
        if (doc.name() == KXMLQLCFixtureHeadChannel)
            addChannel(doc.readElementText().toUInt());
        else
        {
            qWarning() << Q_FUNC_INFO << "Unknown Head tag:" << doc.name();
            doc.skipCurrentElement();
        }
    }

    return true;
}

bool QLCFixtureHead::saveXML(QXmlStreamWriter *doc) const
{
    Q_ASSERT(doc != NULL);

    doc->writeStartElement(KXMLQLCFixtureHead);

    foreach(quint32 index, m_channels)
        doc->writeTextElement(KXMLQLCFixtureHeadChannel, QString::number(index));

    doc->writeEndElement();

    return true;
}

