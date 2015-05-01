/*
  Q Light Controller Plus
  monitorproperties.h

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

#ifndef MONITORPROPERTIES_H
#define MONITORPROPERTIES_H

#include <QPointF>
#include <QColor>
#include <QFont>
#include <QSize>
#include <QHash>

class QDomDocument;
class QDomElement;

class Doc;

/** @addtogroup engine Engine
 * @{
 */

#define KXMLQLCMonitorProperties "Monitor"

struct FixtureItemProperties
{
    FixtureItemProperties()
        : m_position()
        , m_rotation()
        , m_gelColor()
    {
    }

    QPointF m_position;
    ushort m_rotation;
    QColor m_gelColor;
};

struct Fixture3dProperties
{
    Fixture3dProperties()
        : m_posX()
        , m_posY()
        , m_posZ()
        , m_rotX()
        , m_rotY()
        , m_rotZ()
        , m_rotW(1)
        , m_gelColor()
    {
    }

    double m_posX;
    double m_posY;
    double m_posZ;

    double m_rotX;
    double m_rotY;
    double m_rotZ;
    double m_rotW;

    QColor m_gelColor;
};

struct Object3dProperties
{
    Object3dProperties()
        : m_posX()
        , m_posY()
        , m_posZ()
        , m_rotX()
        , m_rotY()
        , m_rotZ()
        , m_rotW(1)
        , m_modelPath()
    {
    }

    double m_posX;
    double m_posY;
    double m_posZ;

    double m_rotX;
    double m_rotY;
    double m_rotZ;
    double m_rotW;

    QString m_modelPath;
};


struct Camera3dProperties
{
    Camera3dProperties()
        : m_eyeX(1000.0f)
        , m_eyeY(1000.0f)
        , m_eyeZ()
        , m_centerX()
        , m_centerY()
        , m_centerZ()
        , m_upX()
        , m_upY()
        , m_upZ(0.0f)
    {
    }

    float m_eyeX;
    float m_eyeY;
    float m_eyeZ;

    float m_centerX;
    float m_centerY;
    float m_centerZ;

    float m_upX;
    float m_upY;
    float m_upZ;
};

class MonitorProperties
{
public:
    MonitorProperties();

    enum DisplayMode { DMX, Graphics, ThreeD };
    enum ChannelStyle { DMXChannels, RelativeChannels };
    enum ValueStyle { DMXValues, PercentageValues };
    enum GridUnits { Meters, Feet };

    void setFont(QFont font) { m_font = font; }
    QFont font() const { return m_font; }

    void setDisplayMode(DisplayMode mode) { m_displayMode = mode; }
    DisplayMode displayMode() const { return m_displayMode; }

    void setChannelStyle(ChannelStyle style) { m_channelStyle = style; }
    ChannelStyle channelStyle() const { return m_channelStyle; }

    void setValueStyle(ValueStyle style) { m_valueStyle = style; }
    ValueStyle valueStyle() const { return m_valueStyle; }

    void setGridSize(QSize size) { m_gridSize = size; }
    QSize gridSize() const { return m_gridSize; }

    void setGridUnits(GridUnits units) { m_gridUnits = units; }
    GridUnits gridUnits() const { return m_gridUnits; }

    void removeFixture(quint32 fid);
    void removeFixture3d(quint32 fid);

    void setFixturePosition(quint32 fid, QPointF pos);
    QPointF fixturePosition(quint32 fid) const { return m_fixtureItems[fid].m_position; }

    void setFixtureRotation(quint32 fid, ushort degrees);
    ushort fixtureRotation(quint32 fid) const { return m_fixtureItems[fid].m_rotation; }

    void setFixtureGelColor(quint32 fid, QColor col);
    QColor fixtureGelColor(quint32 fid) const { return m_fixtureItems[fid].m_gelColor; }

    void setLabelsVisible(bool visible) { m_showLabels = visible; }
    bool labelsVisible() const { return m_showLabels; }

    void setCommonBackgroundImage(QString filename) { m_commonBackgroundImage = filename; }
    QString commonBackgroundImage() const { return m_commonBackgroundImage; }

    void setCustomBackgroundItem(quint32 fid, QString path) { m_customBackgroundImages[fid] = path; }
    void setCustomBackgroundList(QHash<quint32, QString>list) { m_customBackgroundImages = list; }
    void resetCustomBackgroundList() { m_customBackgroundImages.clear(); }
    QHash<quint32, QString> customBackgroundList() const { return m_customBackgroundImages; }
    QString customBackground(quint32 id);

    FixtureItemProperties fixtureProperties(quint32 fid) const { return m_fixtureItems[fid]; }
    void setFixtureProperties(quint32 fid, FixtureItemProperties const & props) { m_fixtureItems[fid] = props; }

    Fixture3dProperties fixture3dProperties(quint32 fid) const { return m_fixture3dItems[fid]; }
    void setFixture3dProperties(quint32 fid, Fixture3dProperties const & props) { m_fixture3dItems[fid] = props; }

    QList<Object3dProperties> & objectProperties() { return m_objects; }

    Camera3dProperties cameraProperties() const { return m_cameraProperties; }
    void setCameraProperties(Camera3dProperties const & props) { m_cameraProperties = props; }

    QList <quint32> fixtureItemsID() const { return m_fixtureItems.keys(); }
    QList <quint32> fixture3dID() const { return m_fixture3dItems.keys(); }

    void reset();

private:
    QFont m_font;
    DisplayMode m_displayMode;
    ChannelStyle m_channelStyle;
    ValueStyle m_valueStyle;
    QSize m_gridSize;
    GridUnits m_gridUnits;
    bool m_showLabels;
    QString m_commonBackgroundImage;
    QHash<quint32, QString> m_customBackgroundImages;
    QHash<quint32, FixtureItemProperties> m_fixtureItems;
    QHash<quint32, Fixture3dProperties> m_fixture3dItems;
    QList<Object3dProperties> m_objects;

    Camera3dProperties m_cameraProperties;

    /*********************************************************************
     * Load & Save
     *********************************************************************/
public:
    /**
     * Load the Monitor properties from the given XML node.
     *
     * @param root An XML subtree containing the Monitor properties
     * @return true if the properties were loaded successfully, otherwise false
     */
    bool loadXML(const QDomElement& root, const Doc* mainDocument);

    /**
     * Save the Monitor properties into an XML document, under the given
     * XML element (tag).
     *
     * @param doc The master XML document to save to.
     * @param wksp_root The workspace root element
     */
    bool saveXML(QDomDocument* doc, QDomElement* wksp_root, const Doc * mainDocument) const;
};

/** @} */

#endif // MONITORPROPERTIES_H
