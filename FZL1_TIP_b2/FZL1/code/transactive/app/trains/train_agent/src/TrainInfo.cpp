/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/TrainInfo.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

///////////////////////////////////////////////////////////
//  TrainInfo.cpp
//  Implementation of the Class TrainInfo
//  Created on:      26-Nov-2003 02:15:47 PM
///////////////////////////////////////////////////////////

#include <sstream>

#include "TrainInfo.h"
#include "ITrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/src/TrainTimer.h"
#include "app/trains/train_agent/src/TrainManager.h"
#include "app/trains/train_agent/src/TrainStatusManager.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;


TrainInfo::TrainInfo(ITrainManager& callback, ITrainNotificationFacade& notifier) : 
m_trainManager(callback),
m_notifier(notifier)
{
    m_trainManager.addDelegate(this);    
}

TrainInfo::~TrainInfo()
{
}

TrainInfo::TrainDetailsNotification::TrainDetailsNotification(unsigned char xtrainID,unsigned long xcurrentLocation,std::string xserviceNumber,bool xisMute,char xtrack,std::string xITSINumber, bool xisLocalisationValid, char xatsValid)
: trainID(xtrainID),
currentLocation(xcurrentLocation),
serviceNumber(xserviceNumber),
isMute(xisMute),
track(static_cast<TrainTypes::Track>(xtrack)),
ITSINumber(xITSINumber),
isLocalisationValid(xisLocalisationValid),
atsValid(static_cast<TrainTypes::AtsValid>(xatsValid))
{
}


bool TrainInfo::delegate(TrainEvent& event)
{
    return false;
#if 0
    TrainEventReader* r = event.getReader();
    bool consumed = true;
    if (dynamic_cast<paAudioSWCabToCabEventReader*>(r) != 0)
    {
        onPAAudioSWCabToCab(event, dynamic_cast<paAudioSWCabToCabEventReader*>(r));
    }
    else if (dynamic_cast<paAudioSWEventReader*>(r) != 0)
    {
        onPAAudioSW(event, dynamic_cast<paAudioSWEventReader*>(r));
    }
    else if (dynamic_cast<InformationTestCallEventReader*>(r) != 0)
    {
        onTestCall(event, dynamic_cast<InformationTestCallEventReader*>(r));
    }
    else if (dynamic_cast<InformationChangeoverstatusEventReader*>(r) != 0)
    {
        onChangeOverStatus(event, dynamic_cast<InformationChangeoverstatusEventReader*>(r));
    }
    else 
    {
        consumed = false;
    }
    return consumed;
#endif
}

TrainInfo::TrainDetailsList TrainInfo::getTrainList()
{
    // get train list and iterate through it, building up our structure
    
    TrainDetailsList theList;

    TrainList trainList = m_trainManager.getTrainStatusManager().getTrainList();
    for (TrainList::iterator i = trainList.begin(); i != trainList.end(); ++i)
    {
        TrainStatusManager& tsm = m_trainManager.getTrainStatusManager();
        theList.push_back(TrainDetailsNotification(
            (*i), // trainID
            tsm.getTrainLocation(*i), // currentLocation
            tsm.getTrainServiceNumber(*i), // serviceNumber
            tsm.isTrainMute(*i), // isMute
            tsm.getTrainDirection(*i), // isInbound
            tsm.getTrainTSI(*i), // ITSINumber
			tsm.isTrainLocalisationValid(*i), // isLocalisationValid
			tsm.getTrainAtsValid(*i) // isValid
            ));
    }

//    return TrainInformation::TrainDetailsList();
	return theList;
}

std::string TrainInfo::getTrainTSI(unsigned char trainID)
{
    // simply call trainstatusmanager

    return m_trainManager.getTrainStatusManager().getTrainTSI(trainID);
}

bool TrainInfo::isTrain(std::string & ITSI)
{
    // delegate to trainstatusmanager
    return m_trainManager.getTrainStatusManager().isTrain(ITSI);
}

std::vector<std::string> TrainInfo::getTrainSelectionList()
{
    // delegate to trainstatusmanager
    return m_trainManager.getTrainStatusManager().getTrainSelectionList();
}

std::vector<unsigned char> TrainInfo::getTrainSelection(std::string selectionName)
{
    // delegate to trainstatusmanager
    return m_trainManager.getTrainStatusManager().getTrainSelection(selectionName);
}


bool TrainInfo::saveTrainSelection(const std::string & selectionName, std::vector<unsigned char> trainList, const std::string & sessionID)
{
    // delegate to trainstatusmanager
    return m_trainManager.getTrainStatusManager().addTrainSelection(selectionName, trainList);
}

void TrainInfo::deleteTrainSelection(const std::string & selectionName, const std::string & sessionID)
{
    // delegate to trainstatusmanager
    m_trainManager.getTrainStatusManager().delTrainSelection(selectionName);
}

