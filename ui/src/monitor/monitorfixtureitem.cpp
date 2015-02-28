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

GraphicsFixtureHead::GraphicsFixtureHead(QGraphicsItem *parent, Fixture & fixture, int h)
    : FixtureHead(fixture, h)
    , m_item(NULL)
    , m_back(NULL)
    , m_panDegrees(0)
    , m_tiltDegrees(0)
{
    m_item = new QGraphicsEllipseItem(parent);
    m_item->setPen(QPen(Qt::white, 1));
    m_item->setBrush(QBrush(Qt::black));

    if (hasMasterDimmer())
    {
        m_back = new QGraphicsEllipseItem(parent);
        m_back->setPen(QPen(Qt::white, 1));
        m_back->setBrush(QBrush(Qt::black));
    }
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
        m_heads.append(new GraphicsFixtureHead(this, *fxi, i));
    }
}

MonitorFixtureItem::~MonitorFixtureItem()
{
    foreach(GraphicsFixtureHead *head, m_heads)
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
		GraphicsFixtureHead * h = m_heads.at(index);
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

    foreach(GraphicsFixtureHead *head, m_heads)
    {
        head->setGelColor(color);
    }
}

void MonitorFixtureItem::updateValues(const QByteArray & ua)
{
    bool needUpdate = false;

    foreach(GraphicsFixtureHead *head, m_heads)
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
    foreach (GraphicsFixtureHead *head, m_heads)
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
