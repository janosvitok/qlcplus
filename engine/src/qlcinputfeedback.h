/*
  Q Light Controller Plus
  qlcinputfeedback.h

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

#ifndef QLCINPUTFEEDBACK_H
#define QLCINPUTFEEDBACK_H

class QXmlStreamWriter;
class QXmlStreamReader;
class QLCInputProfile;
class QString;

/** @addtogroup engine Engine
 * @{
 */

#define KXMLQLCInputFeedback "Feedback"
#define KXMLQLCInputFeedbackName "Name"
#define KXMLQLCInputFeedbackValue "Value"
#define KXMLQLCInputFeedbackMin "Min"
#define KXMLQLCInputFeedbackMax "Max"
#define KXMLQLCInputFeedbackState "State"
#define KXMLQLCInputFeedbackColor "Color"
#define KXMLQLCInputFeedbackOn "On"
#define KXMLQLCInputFeedbackOff "Off"
#define KXMLQLCInputFeedbackBlink "Blink"

class FeedbackValue
{
public:

    FeedbackValue();

    QString const & name() const;
    void setName(QString const & name);

    int min() const;
    void setMin(int min);

    int max() const;
    void setMax(int max);

    /********************************************************************
     * State
     ********************************************************************/
public:
    enum State
    {
        On,
        Off,
        Blink
    };

    /** Set the state (see enum State) */
    void setState(State state);

    /** Get the state */
    State state() const;

    /** Convert the given State to a QString */
    static QString stateToString(State state);

    /** Convert the given QString to a State */
    static State stringToState(const QString& type);

    /** Get a list of available states */
    static QStringList states();

    QColor const & color() const;
    void setColor(QColor const & color);

private:
    QString m_name;
    int m_min;
    int m_max;
    State m_state;
    QColor m_color;
};

class QLCInputFeedback
{
    /********************************************************************
     * Initialization
     ********************************************************************/
public:
    /** Standard constructor */
    QLCInputFeedback();

    /** Copy constructor */
    QLCInputFeedback(const QLCInputFeedback& channel);

    /** Destructor */
    virtual ~QLCInputFeedback();

    /********************************************************************
     * Name
     ********************************************************************/
public:
    /** Set the name of this this channel */
    void setName(const QString& name);

    /** Get the name of this channel */
    QString name() const;

protected:
    QString m_name;

    /********************************************************************
     * Load & Save
     ********************************************************************/
public:
    /**
     * Load this channel's contents from the given XML document
     *
     * @param root An input channel tag
     * @return true if successful, otherwise false
     */
    bool loadXML(QXmlStreamReader &root);

    /**
     * Save this channel's contents to the given XML document, setting the
     * given channel number as the channel's number.
     *
     * @param doc The master XML document to save to
     * @param channelNumber The channel's number in the channel map
     * @return true if successful, otherwise false
     */
    bool saveXML(QXmlStreamWriter *doc, quint32 channelNumber) const;
};

/** @} */

#endif
