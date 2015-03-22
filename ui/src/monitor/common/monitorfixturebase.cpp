/*
  Q Light Controller Plus
  monitorfixturebase.cpp

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

#include <QDebug>

#include "monitorfixturebase.h"
#include "qlcfixturehead.h"
#include "fixture.h"
#include "doc.h"

MonitorFixtureBase::MonitorFixtureBase(Doc *doc, quint32 fid)
    : m_doc(doc)
    , m_fid(fid)
{
    Q_ASSERT(doc != NULL);
}

void MonitorFixtureBase::initialize()
{
    Fixture *fxi = m_doc->fixture(m_fid);
    Q_ASSERT(fxi != NULL);

    m_name = fxi->name();

    for (int i = 0; i < fxi->heads(); i++)
    {
        m_heads.append(createHead(*fxi, i));
    }
}

MonitorFixtureBase::~MonitorFixtureBase()
{
    foreach(MonitorFixtureHead *head, m_heads)
    {
        delete head;
    }
    m_heads.clear();
}

void MonitorFixtureBase::setGelColor(QColor color)
{
    m_gelColor = color;

    foreach(MonitorFixtureHead *head, m_heads)
    {
        head->setGelColor(color);
    }
}

