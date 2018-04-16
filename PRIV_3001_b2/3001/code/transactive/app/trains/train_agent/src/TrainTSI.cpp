/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainTSI.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "TrainException.h"
#include "TrainTSI.h"

using namespace TA_IRS_App;

const std::string TrainTSI::UNKNOWN_TSI = "";

TrainTSI::TrainTSI()
{
	m_trainTSI.known = false;
	m_trainTSI.activeTSI = TrainTypes::TSI_Primary;
    m_trainTSI.TSIs.clear();
    m_trainTSI.TSIs.push_back(UNKNOWN_TSI);
    m_trainTSI.TSIs.push_back(UNKNOWN_TSI);
}

TrainTSI::TrainTSI(const std::string& primaryTSI, const std::string& secondaryTSI,
	TrainTypes::SelectTSI selectTSI)
{
	m_trainTSI.known = false;
	m_trainTSI.activeTSI = selectTSI;
    m_trainTSI.TSIs.clear();
    m_trainTSI.TSIs.push_back(primaryTSI);
    m_trainTSI.TSIs.push_back(secondaryTSI);
}

void TrainTSI::copy(const TrainTSI& trainTSI)
{
    m_trainTSI.known = trainTSI.m_trainTSI.known;
    m_trainTSI.activeTSI = trainTSI.m_trainTSI.activeTSI;
    m_trainTSI.TSIs = trainTSI.m_trainTSI.TSIs;
}


TrainTSI::TrainTSI(const TrainTSI& trainTSI)
{
    copy(trainTSI);
}

TrainTSI TrainTSI::operator=(const TrainTSI& trainTSI)
{
    copy(trainTSI);
    return trainTSI;
}

void TrainTSI::setTSI(TrainTypes::SelectTSI selectTSI, const std::string& TSI)
{
    validateSelectTSI(selectTSI);
    m_trainTSI.TSIs[selectTSI] = TSI;
	m_trainTSI.known = m_trainTSI.TSIs[m_trainTSI.activeTSI] != UNKNOWN_TSI;
}


void TrainTSI::setActiveTSI(TrainTypes::SelectTSI selectTSI)
{
    validateSelectTSI(selectTSI);
	m_trainTSI.activeTSI = selectTSI;
	m_trainTSI.known = m_trainTSI.TSIs[m_trainTSI.activeTSI] != UNKNOWN_TSI;
}

std::string TrainTSI::getActiveTSI() const
{
    if(m_trainTSI.known == false)
    {
        TrainUnknownTSIException("TSI is not known");
    }
	return m_trainTSI.TSIs[m_trainTSI.activeTSI];
}

void TrainTSI::validateSelectTSI(TrainTypes::SelectTSI selectTSI) const
{
    if((selectTSI != TrainTypes::TSI_Primary) && (selectTSI != TrainTypes::TSI_Secondary))
    {
        TrainUnknownTSIException("TSI selection was invalid");
    }
}

std::string TrainTSI::getTSI(TrainTypes::SelectTSI selectTSI) const
{
    validateSelectTSI(selectTSI);
	return m_trainTSI.TSIs[selectTSI];
}

TrainTypes::SelectTSI TrainTSI::getActiveSelectTSI() const
{
	return m_trainTSI.activeTSI;
}

void TrainTSI::getTSI(TrainTSIRec& trainTSI) const
{
	trainTSI.known = m_trainTSI.known;
	trainTSI.activeTSI = m_trainTSI.activeTSI;
	trainTSI.TSIs = m_trainTSI.TSIs;
}
