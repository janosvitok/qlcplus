/*
  Q Light Controller Plus
  monitorfixtureitem.h

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

#ifndef MONITORFIXTUREITEM_H
#define MONITORFIXTUREITEM_H

#include <QGraphicsItem>
#include <QFont>

#include "monitorfixturehead.h"
#include "monitorfixturebase.h"

class Doc;

/** \addtogroup ui_mon_2d 2D View
 * @{
 */

class GraphicsFixtureHead 
    : public MonitorFixtureHead
{
    Q_OBJECT;
public:
    GraphicsFixtureHead(QGraphicsItem *parent, Fixture &fixture, int head);

public:
    QGraphicsEllipseItem *m_item;
    QGraphicsEllipseItem *m_back;
    qreal m_panDegrees;
    qreal m_tiltDegrees;
};

class MonitorFixtureItem : public MonitorFixtureBase, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    MonitorFixtureItem(Doc *doc, quint32 fid);

    MonitorFixtureHead * createHead(Fixture & fixture, int head);

    /** Set the position of this fixture using the monitor measure units */
    void setRealPosition(QPointF pos) { m_realPos = pos; }

    /** Return the position of this fixture express in the monitor measure units */
    QPointF realPosition() { return m_realPos; }

    /** Sets the dimension of this fixture */
    void setSize(QSize size);

    /** Update the fixture values for rendering, passing the
     *  universe array of values */
    void updateValues(const QByteArray& ua);

    void showLabel(bool visible);

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *);

signals:
    void itemDropped(MonitorFixtureItem *);

private:
    /** Width of the item */
    int m_width;

    /** Height of the item */
    int m_height;

    /** Position of the item top-left corner in millimeters */
    QPointF m_realPos;

    /** Flag to show/hide a fixture label */
    bool m_labelVisibility;

    /** Font used if label is visible */
    QFont m_font;

    QRect m_labelRect;
};

/** @} */

#endif // MONITORFIXTUREITEM_H
