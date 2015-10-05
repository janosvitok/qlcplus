/*
  Q Light Controller Plus
  qlcinputfeedback.cpp

  Copyright (c) Jano Svitok

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
#include <QXmlStreamWriter>
#include <QString>
#include <QtXml>
#include <QIcon>

#include "qlcinputfeedback.h"

FeedbackValue::Feedbackvalue()
    : m_min(0)
    , m_max(0)
    , m_state(Off)
{
}

/****************************************************************************
 * Initialization
 ****************************************************************************/

QLCInputChannel::QLCInputChannel()
{
    m_type = Button;
    m_movementType = Absolute;
    m_movementSensitivity = 20;
}

QLCInputChannel::QLCInputChannel(const QLCInputChannel& channel)
{
    m_name = channel.m_name;
    m_type = channel.m_type;
    m_movementType = channel.m_movementType;
    m_movementSensitivity = channel.m_movementSensitivity;
}

QLCInputChannel::~QLCInputChannel()
{
}

/****************************************************************************
 * Type
 ****************************************************************************/

void QLCInputChannel::setType(Type type)
{
    m_type = type;
}

QLCInputChannel::Type QLCInputChannel::type() const
{
    return m_type;
}

QString QLCInputChannel::typeToString(Type type)
{
    QString str;

    switch (type)
    {
    case Button:
        str = QString(KXMLQLCInputChannelButton);
        break;
    case Knob:
        str = QString(KXMLQLCInputChannelKnob);
        break;
    case Slider:
        str = QString(KXMLQLCInputChannelSlider);
        break;
    case NextPage:
        str = QString(KXMLQLCInputChannelPageUp);
        break;
    case PrevPage:
        str = QString(KXMLQLCInputChannelPageDown);
        break;
    case PageSet:
        str = QString(KXMLQLCInputChannelPageSet);
        break;
    default:
        str = QString(KXMLQLCInputChannelNone);
    }

    return str;
}

QLCInputChannel::Type QLCInputChannel::stringToType(const QString& type)
{
    if (type == KXMLQLCInputChannelButton)
        return Button;
    else if (type == KXMLQLCInputChannelKnob)
        return Knob;
    else if (type == KXMLQLCInputChannelSlider)
        return Slider;
    else if (type == KXMLQLCInputChannelPageUp)
        return NextPage;
    else if (type == KXMLQLCInputChannelPageDown)
        return PrevPage;
    else if (type == KXMLQLCInputChannelPageSet)
        return PageSet;
    else
        return NoType;
}

QStringList QLCInputChannel::types()
{
    QStringList list;
    list << KXMLQLCInputChannelSlider;
    list << KXMLQLCInputChannelKnob;
    list << KXMLQLCInputChannelButton;
    list << KXMLQLCInputChannelPageUp;
    list << KXMLQLCInputChannelPageDown;
    list << KXMLQLCInputChannelPageSet;
    return list;
}

QIcon QLCInputChannel::typeToIcon(Type type)
{
    switch (type)
    {
    case Button:
        return QIcon(":/button.png");
    case Knob:
        return QIcon(":/knob.png");
    case Slider:
        return QIcon(":/slider.png");
    case PrevPage:
        return QIcon(":/forward.png");
    case NextPage:
        return QIcon(":/back.png");
    case PageSet:
       return QIcon(":/star.png");
    default:
       return QIcon();
    }
}

QIcon QLCInputChannel::stringToIcon(const QString& str)
{
    return typeToIcon(stringToType(str));
}

QIcon QLCInputChannel::icon() const
{
    return typeToIcon(type());
}

/****************************************************************************
 * Name
 ****************************************************************************/

void QLCInputChannel::setName(const QString& name)
{
    m_name = name;
}

QString QLCInputChannel::name() const
{
    return m_name;
}

/*********************************************************************
 * Slider movement behaviour specific methods
 *********************************************************************/

QLCInputChannel::MovementType QLCInputChannel::movementType() const
{
    return m_movementType;
}

void QLCInputChannel::setMovementType(QLCInputChannel::MovementType type)
{
    m_movementType = type;
}

int QLCInputChannel::movementSensitivity() const
{
    return m_movementSensitivity;
}

void QLCInputChannel::setMovementSensitivity(int value)
{
    m_movementSensitivity = value;
}

/****************************************************************************
 * Load & Save
 ****************************************************************************/

bool QLCInputChannel::loadXML(QXmlStreamReader &root)
{
    if (root.isStartElement() == false || root.name() != KXMLQLCInputChannel)
    {
        qWarning() << Q_FUNC_INFO << "Channel node not found";
        return false;
    }

    while (root.readNextStartElement())
    {
        if (root.name() == KXMLQLCInputChannelName)
        {
            setName(root.readElementText());
        }
        else if (root.name() == KXMLQLCInputChannelType)
        {
            setType(stringToType(root.readElementText()));
        }
        else if (root.name() == KXMLQLCInputChannelMovement)
        {
            if (root.attributes().hasAttribute(KXMLQLCInputChannelSensitivity))
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
                setMovementSensitivity(root.attributes().value(KXMLQLCInputChannelSensitivity).toString().toInt());
#else
                setMovementSensitivity(root.attributes().value(KXMLQLCInputChannelSensitivity).toInt());
#endif
            if (root.readElementText() == KXMLQLCInputChannelRelative)
                setMovementType(Relative);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "Unknown input channel tag" << root.name();
        }
    }

    return true;
}

bool QLCInputChannel::saveXML(QXmlStreamWriter *doc, quint32 channelNumber) const
{
    if (doc == NULL || doc->device() == NULL)
        return false;

    doc->writeStartElement(KXMLQLCInputChannel);
    doc->writeAttribute(KXMLQLCInputChannelNumber,
                        QString("%1").arg(channelNumber));

    doc->writeTextElement(KXMLQLCInputChannelName, m_name);
    doc->writeTextElement(KXMLQLCInputChannelType, typeToString(m_type));

    /* Save only slider's relative movement */
    if (type() == Slider && movementType() == Relative)
    {
        doc->writeStartElement(KXMLQLCInputChannelMovement);
        doc->writeAttribute(KXMLQLCInputChannelSensitivity, QString::number(movementSensitivity()));
        doc->writeCharacters(KXMLQLCInputChannelRelative);
        doc->writeEndElement();
    }

    doc->writeEndElement();
    return true;
}
