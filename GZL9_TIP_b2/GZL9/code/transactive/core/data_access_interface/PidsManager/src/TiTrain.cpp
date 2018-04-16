#include "TiTrain.h"
#include "TiTrainHelper.h"

TiTrain::TiTrain(unsigned long key)
    : m_valid(false),
      m_key(key)
{
}

TiTrain::TiTrain(unsigned long key,
                       int trainNo,
					   std::string displayText)
    : m_valid(true),
      m_key(key),
      m_trainNo(trainNo),
      m_displayText(displayText)
{
}

unsigned long TiTrain::getKey()
{
    return m_key;
}

int TiTrain::getTrainNo()
{
    if(!m_valid)
    {
        reload();
    }
    return m_trainNo;
}

std::string TiTrain::getDisplayText()
{
    if(!m_valid)
    {
        reload();
    }
    return m_displayText;
}

void TiTrain::reload()
{
    TiTrainHelper helper;
    TiTrain t = helper.load(m_key);
    m_trainNo = t.m_trainNo;
    m_displayText = t.m_displayText;
    m_valid = true;
}