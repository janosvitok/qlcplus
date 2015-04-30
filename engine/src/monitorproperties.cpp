/*
  Q Light Controller Plus
  monitorproperties.cpp

  Copyright (c) Massimo Callegari

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

#include <QDebug>
#include <QFont>

#include "monitorproperties.h"
#include "doc.h"
#include <QDomDocument>
#include <QDomElement>

#define KXMLQLCMonitorDisplay "DisplayMode"
#define KXMLQLCMonitorChannels "ChannelStyle"
#define KXMLQLCMonitorValues "ValueStyle"
#define KXMLQLCMonitorFont "Font"
#define KXMLQLCMonitorGrid "Grid"
#define KXMLQLCMonitorGridWidth "Width"
#define KXMLQLCMonitorGridHeight "Height"
#define KXMLQLCMonitorGridUnits "Units"
#define KXMLQLCMonitorShowLabels "ShowLabels"
#define KXMLQLCMonitorCommonBackground "Background"

#define KXMLQLCMonitorCustomBgItem "BackgroundItem"
#define KXMLQLCMonitorCustomBgFuncID "ID"

#define KXMLQLCMonitorFixtureItem "FxItem"
#define KXMLQLCMonitorFixture3dItem "Fx3dItem"
#define KXMLQLCMonitorCamera "Camera"
#define KXMLQLCMonitorCameraXEye "XEye"
#define KXMLQLCMonitorCameraYEye "YEye"
#define KXMLQLCMonitorCameraZEye "ZEye"
#define KXMLQLCMonitorCameraXCenter "XCenter"
#define KXMLQLCMonitorCameraYCenter "YCenter"
#define KXMLQLCMonitorCameraZCenter "ZCenter"
#define KXMLQLCMonitorCameraXUp "XUp"
#define KXMLQLCMonitorCameraYUp "YUp"
#define KXMLQLCMonitorCameraZUp "ZUp"
#define KXMLQLCMonitorFixtureID "ID"
#define KXMLQLCMonitorFixtureXPos "XPos"
#define KXMLQLCMonitorFixtureYPos "YPos"
#define KXMLQLCMonitorFixtureZPos "ZPos"
#define KXMLQLCMonitorFixtureXRot "XRot"
#define KXMLQLCMonitorFixtureYRot "YRot"
#define KXMLQLCMonitorFixtureZRot "ZRot"
#define KXMLQLCMonitorFixtureWRot "WRot"
#define KXMLQLCMonitorFixtureRotation "Rotation"
#define KXMLQLCMonitorFixtureGelColor "GelColor"

MonitorProperties::MonitorProperties()
    : m_font("Arial", 12)
    , m_displayMode(DMX)
    , m_channelStyle(DMXChannels)
    , m_valueStyle(DMXValues)
    , m_gridSize(5, 5)
    , m_gridUnits(Meters)
    , m_showLabels(false)
{
}

void MonitorProperties::removeFixture(quint32 fid)
{
    if (m_fixtureItems.contains(fid))
        m_fixtureItems.take(fid);
}

void MonitorProperties::removeFixture3d(quint32 fid)
{
    if (m_fixture3dItems.contains(fid))
        m_fixture3dItems.take(fid);
}

void MonitorProperties::setFixturePosition(quint32 fid, QPointF pos)
{
    qDebug() << Q_FUNC_INFO << "X:" << pos.x() << "Y:" << pos.y();
    m_fixtureItems[fid].m_position = pos;
}

void MonitorProperties::setFixtureRotation(quint32 fid, ushort degrees)
{
    m_fixtureItems[fid].m_rotation = degrees;
}

void MonitorProperties::setFixtureGelColor(quint32 fid, QColor col)
{
    qDebug() << Q_FUNC_INFO << "Gel color:" << col;
    m_fixtureItems[fid].m_gelColor = col;
}

QString MonitorProperties::customBackground(quint32 id)
{
    if (m_customBackgroundImages.contains(id))
        return m_customBackgroundImages[id];

    return QString();
}

void MonitorProperties::reset()
{
    m_gridSize = QSize(5, 5);
    m_gridUnits = Meters;
    m_showLabels = false;
    m_fixtureItems.clear();
    m_commonBackgroundImage = QString();
}

/*********************************************************************
 * Load & Save
 *********************************************************************/

bool MonitorProperties::loadXML(const QDomElement &root, const Doc * mainDocument)
{
    if (root.tagName() != KXMLQLCMonitorProperties)
    {
        qWarning() << Q_FUNC_INFO << "Monitor node not found";
        return false;
    }

    if (root.hasAttribute(KXMLQLCMonitorDisplay) == false)
    {
        qWarning() << Q_FUNC_INFO << "Cannot determine Monitor display mode !";
        return false;
    }

    setDisplayMode(DisplayMode(root.attribute(KXMLQLCMonitorDisplay).toInt()));
    if (root.hasAttribute(KXMLQLCMonitorShowLabels))
    {
        if (root.attribute(KXMLQLCMonitorShowLabels) == "1")
            setLabelsVisible(true);
        else
            setLabelsVisible(false);
    }

    QDomNode node = root.firstChild();
    while (node.isNull() == false)
    {
        QDomElement tag = node.toElement();

        if (tag.tagName() == KXMLQLCMonitorFont)
        {
            QFont fn;
            fn.fromString(tag.text());
            setFont(fn);
        }
        else if (tag.tagName() == KXMLQLCMonitorChannels)
            setChannelStyle(ChannelStyle(tag.text().toInt()));
        else if (tag.tagName() == KXMLQLCMonitorValues)
            setValueStyle(ValueStyle(tag.text().toInt()));
        else if (tag.tagName() == KXMLQLCMonitorCommonBackground)
            setCommonBackgroundImage(mainDocument->denormalizeComponentPath(tag.text()));
        else if (tag.tagName() == KXMLQLCMonitorCustomBgItem)
        {
            if (tag.hasAttribute(KXMLQLCMonitorCustomBgFuncID))
            {
                quint32 fid = tag.attribute(KXMLQLCMonitorCustomBgFuncID).toUInt();
                setCustomBackgroundItem(fid, mainDocument->denormalizeComponentPath(tag.text()));
            }
        }
        else if (tag.tagName() == KXMLQLCMonitorGrid)
        {
            int w = 5, h = 5;
            if (tag.hasAttribute(KXMLQLCMonitorGridWidth))
                w = tag.attribute(KXMLQLCMonitorGridWidth).toInt();
            if (tag.hasAttribute(KXMLQLCMonitorGridHeight))
                h = tag.attribute(KXMLQLCMonitorGridHeight).toInt();
            if (tag.hasAttribute(KXMLQLCMonitorGridUnits))
                setGridUnits(GridUnits(tag.attribute(KXMLQLCMonitorGridUnits).toInt()));

            setGridSize(QSize(w, h));
        }
        else if (tag.tagName() == KXMLQLCMonitorFixtureItem)
        {
            if (tag.hasAttribute(KXMLQLCMonitorFixtureID))
            {
                quint32 fid = tag.attribute(KXMLQLCMonitorFixtureID).toUInt();
                QPointF pos(0, 0);
                if (tag.hasAttribute(KXMLQLCMonitorFixtureXPos))
                    pos.setX(tag.attribute(KXMLQLCMonitorFixtureXPos).toDouble());
                if (tag.hasAttribute(KXMLQLCMonitorFixtureYPos))
                    pos.setY(tag.attribute(KXMLQLCMonitorFixtureYPos).toDouble());                
                setFixturePosition(fid, pos);

                if (tag.hasAttribute(KXMLQLCMonitorFixtureRotation))
                    setFixtureRotation(fid, tag.attribute(KXMLQLCMonitorFixtureRotation).toUShort());

                if (tag.hasAttribute(KXMLQLCMonitorFixtureGelColor))
                    setFixtureGelColor(fid, QColor(tag.attribute(KXMLQLCMonitorFixtureGelColor)));
            }
        }
        else if (tag.tagName() == KXMLQLCMonitorFixture3dItem)
        {
            if (tag.hasAttribute(KXMLQLCMonitorFixtureID))
            {
                quint32 fid = tag.attribute(KXMLQLCMonitorFixtureID).toUInt();

                Fixture3dProperties p = fixture3dProperties(fid);

                if (tag.hasAttribute(KXMLQLCMonitorFixtureXPos))
                    p.m_posX = tag.attribute(KXMLQLCMonitorFixtureXPos).toDouble();
                if (tag.hasAttribute(KXMLQLCMonitorFixtureYPos))
                    p.m_posY = tag.attribute(KXMLQLCMonitorFixtureYPos).toDouble();
                if (tag.hasAttribute(KXMLQLCMonitorFixtureZPos))
                    p.m_posZ = tag.attribute(KXMLQLCMonitorFixtureZPos).toDouble();
                if (tag.hasAttribute(KXMLQLCMonitorFixtureXRot))
                    p.m_rotX = tag.attribute(KXMLQLCMonitorFixtureXRot).toDouble();
                if (tag.hasAttribute(KXMLQLCMonitorFixtureYRot))
                    p.m_rotY = tag.attribute(KXMLQLCMonitorFixtureYRot).toDouble();
                if (tag.hasAttribute(KXMLQLCMonitorFixtureZRot))
                    p.m_rotZ = tag.attribute(KXMLQLCMonitorFixtureZRot).toDouble();
                if (tag.hasAttribute(KXMLQLCMonitorFixtureWRot))
                    p.m_rotW = tag.attribute(KXMLQLCMonitorFixtureWRot).toDouble();

                if (tag.hasAttribute(KXMLQLCMonitorFixtureGelColor))
                    p.m_gelColor = QColor(tag.attribute(KXMLQLCMonitorFixtureGelColor));
                setFixture3dProperties(fid, p);
            }
        }
        else if (tag.tagName() == KXMLQLCMonitorCamera)
        {
            Camera3dProperties p = cameraProperties();

            if (tag.hasAttribute(KXMLQLCMonitorCameraXEye))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraXEye).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraYEye))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraYEye).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraZEye))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraZEye).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraXCenter))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraXCenter).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraYCenter))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraYCenter).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraZCenter))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraZCenter).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraXUp))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraXUp).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraYUp))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraYUp).toFloat();
            if (tag.hasAttribute(KXMLQLCMonitorCameraZUp))
                p.m_eyeX = tag.attribute(KXMLQLCMonitorCameraZUp).toFloat();


            setCameraProperties(p);
        }

        node = node.nextSibling();
    }
    return true;
}

bool MonitorProperties::saveXML(QDomDocument *doc, QDomElement *wksp_root, const Doc * mainDocument) const
{
    Q_ASSERT(doc != NULL);
    Q_ASSERT(wksp_root != NULL);

    QDomElement root;
    QDomElement tag;
    QDomText text;

    /* Create the master Monitor node */
    root = doc->createElement(KXMLQLCMonitorProperties);
    root.setAttribute(KXMLQLCMonitorDisplay, displayMode());
    root.setAttribute(KXMLQLCMonitorShowLabels, labelsVisible());
    wksp_root->appendChild(root);


    tag = doc->createElement(KXMLQLCMonitorFont);
    root.appendChild(tag);
    text = doc->createTextNode(font().toString());
    tag.appendChild(text);

    tag = doc->createElement(KXMLQLCMonitorChannels);
    root.appendChild(tag);
    text = doc->createTextNode(QString::number(channelStyle()));
    tag.appendChild(text);

    tag = doc->createElement(KXMLQLCMonitorValues);
    root.appendChild(tag);
    text = doc->createTextNode(QString::number(valueStyle()));
    tag.appendChild(text);

    if (commonBackgroundImage().isEmpty() == false)
    {
        tag = doc->createElement(KXMLQLCMonitorCommonBackground);
        root.appendChild(tag);
        text = doc->createTextNode(mainDocument->normalizeComponentPath(commonBackgroundImage()));
        tag.appendChild(text);
    }
    else if(customBackgroundList().isEmpty() == false)
    {
        QHashIterator <quint32, QString> it(customBackgroundList());
        while (it.hasNext() == true)
        {
            it.next();
 
            tag = doc->createElement(KXMLQLCMonitorCustomBgItem);
            root.appendChild(tag);
            quint32 fid = it.key();
            tag.setAttribute(KXMLQLCMonitorCustomBgFuncID, fid);
            text = doc->createTextNode(mainDocument->normalizeComponentPath(it.value()));
            tag.appendChild(text);
        }
    }

    tag = doc->createElement(KXMLQLCMonitorGrid);
    tag.setAttribute(KXMLQLCMonitorGridWidth, gridSize().width());
    tag.setAttribute(KXMLQLCMonitorGridHeight, gridSize().height());
    tag.setAttribute(KXMLQLCMonitorGridUnits, gridUnits());
    root.appendChild(tag);

    foreach (quint32 fid, fixtureItemsID())
    {
        QPointF pos = fixturePosition(fid);
        tag = doc->createElement(KXMLQLCMonitorFixtureItem);
        tag.setAttribute(KXMLQLCMonitorFixtureID, fid);
        tag.setAttribute(KXMLQLCMonitorFixtureXPos, QString::number(pos.x()));
        tag.setAttribute(KXMLQLCMonitorFixtureYPos, QString::number(pos.y()));
        if (fixtureRotation(fid) != 0)
            tag.setAttribute(KXMLQLCMonitorFixtureRotation, QString::number(fixtureRotation(fid)));

        QColor col = fixtureGelColor(fid);
        if (col.isValid())
            tag.setAttribute(KXMLQLCMonitorFixtureGelColor, col.name());
        root.appendChild(tag);
    }

    foreach (quint32 fid, fixture3dID())
    {
        Fixture3dProperties p = fixture3dProperties(fid);

        tag = doc->createElement(KXMLQLCMonitorFixture3dItem);
        tag.setAttribute(KXMLQLCMonitorFixtureID, fid);
        tag.setAttribute(KXMLQLCMonitorFixtureXPos, QString::number(p.m_posX));
        tag.setAttribute(KXMLQLCMonitorFixtureYPos, QString::number(p.m_posY));
        tag.setAttribute(KXMLQLCMonitorFixtureZPos, QString::number(p.m_posZ));
        tag.setAttribute(KXMLQLCMonitorFixtureXRot, QString::number(p.m_rotX));
        tag.setAttribute(KXMLQLCMonitorFixtureYRot, QString::number(p.m_rotY));
        tag.setAttribute(KXMLQLCMonitorFixtureZRot, QString::number(p.m_rotZ));
        tag.setAttribute(KXMLQLCMonitorFixtureWRot, QString::number(p.m_rotW));

        if (p.m_gelColor.isValid())
            tag.setAttribute(KXMLQLCMonitorFixtureGelColor, p.m_gelColor.name());
        root.appendChild(tag);
    }

    tag = doc->createElement(KXMLQLCMonitorCamera);
    tag.setAttribute(KXMLQLCMonitorCameraXEye, QString::number(m_cameraProperties.m_eyeX));
    tag.setAttribute(KXMLQLCMonitorCameraYEye, QString::number(m_cameraProperties.m_eyeY));
    tag.setAttribute(KXMLQLCMonitorCameraZEye, QString::number(m_cameraProperties.m_eyeZ));
    tag.setAttribute(KXMLQLCMonitorCameraXCenter, QString::number(m_cameraProperties.m_centerX));
    tag.setAttribute(KXMLQLCMonitorCameraYCenter, QString::number(m_cameraProperties.m_centerY));
    tag.setAttribute(KXMLQLCMonitorCameraZCenter, QString::number(m_cameraProperties.m_centerZ));
    tag.setAttribute(KXMLQLCMonitorCameraXUp, QString::number(m_cameraProperties.m_upX));
    tag.setAttribute(KXMLQLCMonitorCameraYUp, QString::number(m_cameraProperties.m_upY));
    tag.setAttribute(KXMLQLCMonitorCameraZUp, QString::number(m_cameraProperties.m_upZ));

    root.appendChild(tag);

    return true;
}
