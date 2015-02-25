/*
  Q Light Controller Plus
  monitorfixtureitem.cpp

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

#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QPainter>
#include <qmath.h>
#include <QCursor>
#include <QDebug>

#include "monitorfixtureitem.h"
#include "qlcfixturehead.h"
#include "qlcfixturemode.h"
#include "qlcfixturedef.h"
#include "qlccapability.h"
#include "fixture.h"
#include "doc.h"

#define MOVEMENT_THICKNESS    3
#define STROBE_PERIOD 30

FixtureHead::FixtureHead(QGraphicsItem *parent, Fixture & fixture, int h)
    : m_masterDimmer(QLCChannel::invalid())
    , m_panChannel(QLCChannel::invalid())
    , m_panMaxDegrees(540)
    , m_tiltChannel(QLCChannel::invalid())
    , m_tiltMaxDegrees(270)
    , m_strobePhase(0)
    , m_item(NULL)
    , m_back(NULL)
    , m_panDegrees(0)
    , m_tiltDegrees(0)
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
                        || cap->name().contains("blackout", Qt::CaseInsensitive)
                        || cap->name().contains("off", Qt::CaseInsensitive))                       {
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
                // handle case when the channel has only one capability 0-255 strobe:
                // make 0 Open to avoid blinking
                if (ch->capabilities().size() <= 1)
                    values[0] = FixtureHead::Open;

                m_shutterValues[shutter + fixture.address()] = values;
                m_shutterChannels << (shutter + fixture.address());
            }
        }
    }

    m_item = new QGraphicsEllipseItem(parent);
    m_item->setPen(QPen(Qt::white, 1));
    m_item->setBrush(QBrush(Qt::black));

    if (m_masterDimmer != QLCChannel::invalid())
    {
        m_back = new QGraphicsEllipseItem(parent);
        m_back->setPen(QPen(Qt::white, 1));
        m_back->setBrush(QBrush(Qt::black));
    }
}

QColor FixtureHead::computeColor(const QByteArray & ua)
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
uchar FixtureHead::computeAlpha(const QByteArray & ua)
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

qreal FixtureHead::computeTiltPosition(const QByteArray & ua)
{
    qreal value = 0.0;
    if (m_tiltChannel == QLCChannel::invalid())
        return value;

    if (m_tiltChannel < (quint32)ua.size())
    {
        value = ((qreal)ua.at(m_tiltChannel) * m_tiltMaxDegrees) / (256.0 - 1/256) - (m_tiltMaxDegrees / 2);
    }
    else
    {
        value = -(m_tiltMaxDegrees / 2);
    }

    //qDebug() << "TILT degrees:" << value;
    return value;
}

qreal FixtureHead::computePanPosition(const QByteArray & ua)
{
    qreal value = 0.0;
    if (m_panChannel == QLCChannel::invalid())
        return value;

    if (m_panChannel < (quint32)ua.size())
    {
        value = ((qreal)ua.at(m_panChannel) * m_panMaxDegrees) / (256.0 - 1/256) - (m_panMaxDegrees / 2);
    }
    else
    {
        value = -(m_panMaxDegrees / 2);
    }

    //qDebug() << "PAN degrees:" << value;
    return value;
}

MonitorFixtureItem::MonitorFixtureItem(Doc *doc, quint32 fid)
    : m_doc(doc)
    , m_fid(fid)
    , m_labelVisibility(false)
{
    Q_ASSERT(doc != NULL);

    setCursor(Qt::OpenHandCursor);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    Fixture *fxi = m_doc->fixture(fid);
    Q_ASSERT(fxi != NULL);

    m_name = fxi->name();

    setToolTip(m_name);

    m_font = qApp->font();
    m_font.setPixelSize(8);

    for (int i = 0; i < fxi->heads(); i++)
    {
        m_heads.append(new FixtureHead(this, *fxi, i));
    }
}

MonitorFixtureItem::~MonitorFixtureItem()
{
    foreach(FixtureHead *head, m_heads)
    {
        delete head;
    }
    m_heads.clear();
}

void MonitorFixtureItem::setSize(QSize size)
{
    prepareGeometryChange();
    m_width = size.width();
    m_height = size.height();

    if (m_width < 5 || m_height < 5)
        return;

    // if this fixture has a pan or tilt channel,
    // the head area has to be reduced to
    // leave space to movements representation
    int headsWidth = m_width;
    int headsHeight = m_height;

    // calculate the diameter of every single head
    double headArea = (headsWidth * headsHeight) / m_heads.count();
    double headSide = sqrt(headArea);
    int columns = (headsWidth / headSide) + 0.5;
    int rows = (headsHeight / headSide) + 0.5;

    // dirty workaround to correctly display right columns on one row
    if (rows == 1)
        columns = m_heads.count();
    if (columns == 1)
        rows = m_heads.count();

    //qDebug() << "Fixture columns:" << columns;

    if (columns > m_heads.count())
        columns = m_heads.count();

    if (rows < 1)
        rows = 1;
    if (columns < 1)
        columns = 1;

    double cellWidth = headsWidth / columns;
    double cellHeight = headsHeight / rows;
    double headDiam = (cellWidth < cellHeight) ? cellWidth : cellHeight;
    
    int ypos = (cellHeight - headDiam) / 2;
    for (int i = 0; i < rows; i++)
    {
        int xpos = (cellWidth - headDiam) / 2;
        for (int j = 0; j < columns; j++)
        {
            int index = i * columns + j;
            if (index < m_heads.size())
            {
		FixtureHead * h = m_heads.at(index);
                QGraphicsEllipseItem *head = h->m_item;
                head->setRect(xpos, ypos, headDiam, headDiam);

                if (h->hasPan())
                {
                    head->setRect(head->rect().adjusted(MOVEMENT_THICKNESS + 1, MOVEMENT_THICKNESS + 1, -MOVEMENT_THICKNESS - 1, -MOVEMENT_THICKNESS - 1));
                }
                if (h->hasTilt())
                {
                    head->setRect(head->rect().adjusted(MOVEMENT_THICKNESS + 1, MOVEMENT_THICKNESS + 1, -MOVEMENT_THICKNESS - 1, -MOVEMENT_THICKNESS - 1));
                }
 
                head->setZValue(2);
                QGraphicsEllipseItem *back = m_heads.at(index)->m_back;
                if (back != NULL)
                {
                    back->setRect(head->rect());
                    back->setZValue(1);
                }
            }
            xpos += cellWidth;
        }
        ypos += cellHeight;
    }

    QFontMetrics fm(m_font);
    m_labelRect = fm.boundingRect(QRect(-10, m_height + 2, m_width + 20, 30),
                                  Qt::AlignHCenter | Qt::TextWrapAnywhere, m_name);

    setTransformOriginPoint(m_width / 2, m_height / 2);
    update();
}

void MonitorFixtureItem::setGelColor(QColor color)
{
    m_gelColor = color;

    foreach(FixtureHead *head, m_heads)
    {
        head->setGelColor(color);
    }
}

void MonitorFixtureItem::updateValues(const QByteArray & ua)
{
    bool needUpdate = false;

    foreach(FixtureHead *head, m_heads)
    {

        QColor col = head->computeColor(ua);
        col.setAlpha(head->computeAlpha(ua));
        head->m_item->setBrush(QBrush(col));

        qreal const panPosition = head->computePanPosition(ua);
        qreal const tiltPosition = head->computeTiltPosition(ua);

        if (head->m_panDegrees != panPosition || head->m_tiltDegrees != tiltPosition)
        {
            head->m_panDegrees = panPosition;
            head->m_tiltDegrees = tiltPosition;
            needUpdate = true;
        }
    }
    if (needUpdate)
        update();
}

void MonitorFixtureItem::showLabel(bool visible)
{
    prepareGeometryChange();
    m_labelVisibility = visible;
    update();
}

QRectF MonitorFixtureItem::boundingRect() const
{
    if (m_labelVisibility)
        return QRectF(-10, 0, m_width + 20, m_height + m_labelRect.height() + 2);
    else
        return QRectF(0, 0, m_width, m_height);
}

void MonitorFixtureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QColor defColor = Qt::darkGray;

    if (this->isSelected() == true)
        defColor = Qt::yellow;

    painter->setPen(QPen(defColor, 1));

    // draw item background
    painter->setBrush(QBrush(QColor(33, 33, 33)));
    painter->drawRect(0, 0, m_width, m_height);
    foreach (FixtureHead *head, m_heads)
    {
        QRectF rect = head->m_item->rect();

        if (head->hasTilt())
        {
            rect.adjust(-MOVEMENT_THICKNESS, -MOVEMENT_THICKNESS, MOVEMENT_THICKNESS, MOVEMENT_THICKNESS);
            
            painter->setPen(QPen(defColor, MOVEMENT_THICKNESS));
            painter->drawArc(rect, 270 * 16 - head->tiltRange() * 16 / 2 - 8, 16);
            painter->drawArc(rect, 270 * 16 + head->tiltRange() * 16 / 2 - 8, 16);
            painter->setPen(QPen(QColor("turquoise"), MOVEMENT_THICKNESS));
            painter->drawArc(rect, 270 * 16, - head->m_tiltDegrees * 16);
        }

        if (head->hasPan())
        {
            rect.adjust(-MOVEMENT_THICKNESS, -MOVEMENT_THICKNESS, MOVEMENT_THICKNESS, MOVEMENT_THICKNESS);

            painter->setPen(QPen(defColor, MOVEMENT_THICKNESS));
            painter->drawArc(rect, 270 * 16 - head->panRange() * 16 / 2 - 8, 16);
            painter->drawArc(rect, 270 * 16 + head->panRange() * 16 / 2 - 8, 16);
            painter->setPen(QPen(QColor("purple"), MOVEMENT_THICKNESS));
            painter->drawArc(rect, 270 * 16, - head->m_panDegrees * 16);
        }
    }

    if (m_labelVisibility)
    {
        painter->setFont(m_font);
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(QBrush(QColor(33, 33, 33)));
        painter->drawRoundedRect(m_labelRect, 2, 2);
        painter->setPen(QPen(Qt::white, 1));
        painter->drawText(m_labelRect, Qt::AlignHCenter | Qt::TextWrapAnywhere, m_name);
    }
}

void MonitorFixtureItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    this->setSelected(true);
}

void MonitorFixtureItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    qDebug() << Q_FUNC_INFO << "mouse RELEASE event - <" << event->pos().toPoint().x() << "> - <" << event->pos().toPoint().y() << ">";
    setCursor(Qt::OpenHandCursor);
    emit itemDropped(this);
}

void MonitorFixtureItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *)
{
}
