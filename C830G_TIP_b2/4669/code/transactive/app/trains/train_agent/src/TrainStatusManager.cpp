/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainStatusManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

///////////////////////////////////////////////////////////
//  TrainStatusManager.cpp
//  Implementation of the Class TrainStatusManager
//  Created on:      05-Dec-2003 03:41:41 PM
///////////////////////////////////////////////////////////


#include "TrainStatusManager.h"
#include "ITrainStatusUpdateMonitor.h"
#include "ITrainNotificationFacade.h"
#include "ITrainManager.h"
#include "app/trains/train_agent/src/TARethrow.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TTDFrame.h"

// for tsi update strategy...
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEventReader.h"
#include "app/trains/train_agent/TrainProtocolLibrary/src/miscTestCallEventReader.h"

#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"

#include <utility>
#include <algorithm>
#include <functional>

using namespace TA_IRS_App;
using namespace TA_IRS_App::TrainStatus;

const struct PECPoint TrainStatusManager::emptyPoint = { 0, 0, TrainTypes::PEC_UNASSIGNED, 0, false };

TrainStatusManager::TrainStatusManager(ITrainStatusUpdateMonitor& callback,
									   ITrainNotificationFacade& notifier,
									   unsigned int autoContinueAckLimit)
					: m_callback(callback),
					m_notifier(notifier),
					m_autoContinueAckLimit(autoContinueAckLimit),
					m_tsiUpdateStrategy(0)
{
	FUNCTION_ENTRY("Constructor");

	FUNCTION_EXIT;
}


bool TrainStatusManager::delegate(std::vector<TrainStatus::TrainInformation>& updateList)
{
    bool consumed = true;

	onTrainStateUpdate(updateList);
    
	return consumed;
}


void TrainStatusManager::setTSIUpdateStrategy(TrainTSIUpdateStrategy * strategy)
{
	if (m_tsiUpdateStrategy != 0)
	{
		delete m_tsiUpdateStrategy;
	}
	m_tsiUpdateStrategy = strategy;
}

TrainStatusManager::~TrainStatusManager()
{
	FUNCTION_ENTRY("Destructor");
	
	delete m_tsiUpdateStrategy; m_tsiUpdateStrategy = 0;

	TrainInformationMap::iterator   it;
	TA_IRS_App::TrainStatus::TrainInformation*               trainInfo;

	// Release the objects in the map
	for (it=m_trainInfoMap.begin();it!=m_trainInfoMap.end();it++)
	{
		trainInfo = (*it).second;
		delete trainInfo;
		(*it).second = NULL;
	}
	m_trainInfoMap.clear();

	FUNCTION_EXIT;
}

void TrainStatusManager::setTrainTSI(TrainEvent & event, TrainEventReader * reader, bool isSynchronisation /*=false*/)
{
    m_tsiUpdateStrategy->setTSI(event, reader, isSynchronisation);

	//if we dont know the train then we need to set CCTV to a known state
	//its no good doing this in addTrainInfo, because if addTrainInfo was 
	//called by the ATS saying a new train is there, we probably cant communicate
	//with it as there wont be a TSI, so it must be done once a TSI is received for it here

	try
	{
		if(!isCctvInitialised(event.getTrainID()))
		{
			defaultTrainCCTVSettings(event.getTrainID());
		}
	}
	catch (TrainInvalidTrainException&)
	{
        // do nothing
        // if we get here the train has no TSI
        // this will only happen if the TSI updat strategy is set to
        // strict wakeup
	}
}

void TrainStatusManager::setTrainTSI(unsigned int trainID, const std::string & TSI, TrainTypes::MPUState MPU, bool isSynchronisation /*=false*/)
{
	bool    isUpdated = false;
	TrainInformation copyOfInfo;

	if (MPU == TrainTypes::MPU_Unknown)
	{
		TA_THROW(TrainInvalidParameterException("Invalid MPU"));	
	}

	// Nest for scope of lock
	{
		// Lock the store while we read it
		TA_THREADGUARD(m_storeLock);

		TrainInformation* trainInfo = getOrAddTrainInformation(trainID);
	
		if ( (static_cast<TrainTypes::MPUState>(trainInfo->activeMPU) != MPU) || (trainInfo->mpuTSI[MPU] != TSI) )
		{
			/// @todo Add some checks to prevent duplicate TSI's 
			trainInfo->mpuTSI[MPU] = TSI;

			/// @todo don't do this here for strict TSI handling, rely on onChangeover processing
			if (MPU == TrainTypes::MPU_Primary)
			{
				trainInfo->activeMPU = TrainTypes::MPU_Primary;
			} 
			else if (MPU == TrainTypes::MPU_Secondary)
			{
				trainInfo->activeMPU = TrainTypes::MPU_Secondary;
			}
			else // default to something sane
			{
				trainInfo->activeMPU = TrainTypes::MPU_Unknown;
			}

			std::ostringstream s;
			s << "Setting ITSI for Train " << static_cast<int>(trainID) 
				<< " MPU " << MPU << " ITSI " << TSI 
				<< " activeMPU is " << trainInfo->activeMPU << std::ends;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  
				TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

			{
				std::ostringstream s;
				s << printTrainTSIAssignment(trainID);
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}

			isUpdated = true;
			copyOfInfo = *trainInfo;
		}
	}

	// Now do the call back (outside scope of lock)
	if (isUpdated&&!isSynchronisation)
	{
		m_callback.onTrainStatusUpdate(trainID, copyOfInfo, ITrainStatusUpdateMonitor::TRAIN_ACTIVE_TSI_FIELD);
		// also send the broadcast comms message
		onStateChange(trainID, syncTSI);
	}
}

bool TrainStatusManager::setTrainLocation(unsigned int trainID,unsigned long locationKey, bool force /*=false*/, bool isSynchronisation /*=false*/)
{
	bool status = false;
	TrainInformation copyOfInfo;

	{
		// Lock the store while we read it
		TA_THREADGUARD(m_storeLock);
		
		TrainInformation*               trainInfo = 0;
		
		// Get (or create the train
		try
		{
			trainInfo = getTrainInformation(trainID);
			// generally, if the train is already present, don't adjust the location (ATS provides it)
			if (0 == trainInfo->currentLocation || 
				true == force && trainInfo->currentLocation != locationKey)
			{
				{
					std::stringstream s;
					s << "Modifying train entry [TrainID=" << static_cast<int>(trainID) << "] [location=" << locationKey << "] " << std::ends;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
				}
				trainInfo->currentLocation = locationKey;
				status = true;
				copyOfInfo = *trainInfo;
			}
			else
			{
				std::stringstream s;
				s << "Not modifying train entry [TrainID=" << static_cast<int>(trainID) << "] [current location=" << trainInfo->currentLocation << "] " << std::ends;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());		
			}
		}
		catch (TrainInvalidTrainException&)
		{
			{
				std::stringstream s;
				s << "Adding new train entry [TrainID=" << static_cast<int>(trainID) << "] [location=" << locationKey << "] " << std::ends;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}
			trainInfo = addTrainInformation(trainID);
			trainInfo->currentLocation = locationKey;
			status = true;
			copyOfInfo = *trainInfo;
		}
		
		
	}

	if (true == status) 
	{
		m_callback.onTrainStatusUpdate(trainID, copyOfInfo, ITrainStatusUpdateMonitor::TRAIN_ACTIVE_TSI_FIELD);
	}
	return status;
}


/**
 * This method is called by the servant to notify the object of changes to the
 * train information from ATS. 
 */
void TrainStatusManager::setTrainDetails(unsigned int trainID, unsigned long currentLocation, double currentCctvZone, std::string serviceNumber, TrainTypes::Track track, bool isMute, bool localisationValid)
{
	FUNCTION_ENTRY("setTrainDetails");

	TrainInformation copyOfInfo;
	
	{
		
		// Lock the store while we read it
		TA_THREADGUARD(m_storeLock);
		
		TrainInformation* trainInfo = getOrAddTrainInformation(trainID);
		
		trainInfo->currentLocation = currentLocation;
		trainInfo->currentCctvZone = currentCctvZone;
		trainInfo->serviceNumber = serviceNumber;
		trainInfo->isMute = isMute;
		trainInfo->track = track;
		trainInfo->isLocalisationValid = localisationValid;
		trainInfo->atsValid = TrainTypes::AtsValid_Valid;
		copyOfInfo = *trainInfo;
	}
	
	m_callback.onTrainStatusUpdate(trainID, copyOfInfo, ITrainStatusUpdateMonitor::TRAIN_ACTIVE_TSI_FIELD);
    FUNCTION_EXIT;
}

void TrainStatusManager::invalidateTrain(unsigned int trainID)
{
	FUNCTION_ENTRY("invalidateTrain");

	try
	{
		TrainInformation copyOfInfo;

		{
			TA_THREADGUARD(m_storeLock);
	
			TrainInformation* trainInfo = getTrainInformation(trainID);
			trainInfo->atsValid = TrainTypes::AtsValid_Invalid;

			std::ostringstream s;
			s << "Invalidating train [" << static_cast<int>(trainID) << "]";
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
        
			// TD 4007 & 8487
			// wouldnt it be nice to let everyone else know that the train is now invalid
        
			copyOfInfo = *trainInfo;
		}

        m_callback.onTrainStatusUpdate(trainID, copyOfInfo, ITrainStatusUpdateMonitor::TRAIN_ACTIVE_TSI_FIELD);
	}
	catch (TrainInvalidTrainException&)
	{
		// nothing to invalidate
	}

    FUNCTION_EXIT;
}

void TrainStatusManager::setAtsValidityToUnknown(unsigned int trainID)
{
	FUNCTION_ENTRY("setAtsValidityToUnknown");

	try
	{
		TrainInformation copyOfInfo;
		bool update = false;

		{
			TA_THREADGUARD(m_storeLock);
	
			TrainInformation* trainInfo = getTrainInformation(trainID);
			
			if (trainInfo->atsValid == TrainTypes::AtsValid_Invalid)
			{
				trainInfo->atsValid = TrainTypes::AtsValid_Unknown;

				std::ostringstream s;
				s << "Updating ats validity to unknown [" << static_cast<int>(trainID) << "]";
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
        
				// TD 4007 & 8487
				// wouldnt it be nice to let everyone else know that the train is now unknown
        
				copyOfInfo = *trainInfo;
				update = true;
			}
		}

        if (update)
		{
			m_callback.onTrainStatusUpdate(trainID, copyOfInfo, ITrainStatusUpdateMonitor::TRAIN_ACTIVE_TSI_FIELD);
		}
	}
	catch (TrainInvalidTrainException&)
	{
		// nothing to make unknown
	}

    FUNCTION_EXIT;
}


/**
 * Get a list of trains that we know about (that are valid, according to ATS).
 */
TrainList TrainStatusManager::getTrainList(bool includeInvalid /*= false*/)
{
	FUNCTION_ENTRY("getTrainList");
	
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

	TrainList       trainList;
	TrainInformationMap::iterator   it;
	TrainInformation*               trainInfo;

	// Release the objects in the map
	for (it=m_trainInfoMap.begin();it!=m_trainInfoMap.end();it++)
	{
		trainInfo = (*it).second;
		if ((true == includeInvalid) || (TrainTypes::AtsValid_Invalid != trainInfo->atsValid))
		{
			trainList.push_back(trainInfo->trainID);
		}
	}

	FUNCTION_EXIT;
	return trainList;
}


/**
 * Get the current location for the train
 */
std::string TrainStatusManager::getTrainTSI(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainTSI");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
//	TA_ASSERT(trainInfo->activeMPU == 0 || trainInfo->activeMPU == 1, "Invalid MPU");
	FUNCTION_EXIT;
	/// @todo clean this up
	std::string result = trainInfo->mpuTSI[(trainInfo->activeMPU == TrainTypes::MPU_Unknown) ? TrainTypes::MPU_Primary : trainInfo->activeMPU];
	std::ostringstream s;
	s << "TIP5 GetTrainTSI is about to return [" << result << "] mpu [" << trainInfo->activeMPU << "]"
		<< " for train [" << static_cast<unsigned int>(trainID) << "]" << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
	return result;
}

std::string TrainStatusManager::getTrainTSI(unsigned int trainID, TrainTypes::MPUState mpuState)
{
	FUNCTION_ENTRY("getTrainTSI");

	std::string tsi;
	
	try
	{
		TA_THREADGUARD(m_storeLock);

		TrainInformation* trainInfo = getTrainInformation(trainID);

		switch(mpuState)
		{
			case TrainTypes::MPU_Primary:
			{
				tsi = trainInfo->mpuTSI[mpuState];
			}
			case TrainTypes::MPU_Secondary:
			{
				tsi = trainInfo->mpuTSI[mpuState];
			}
		}
	}
	catch(TrainInvalidTrainException&)
	{
		// do nothing, the method will return an empty string for the tsi
	}

	FUNCTION_EXIT;
	return tsi;
}

unsigned int TrainStatusManager::getTrainMPUState(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainMPUState");

	unsigned int mpuState = TrainTypes::MPU_Unknown;

	try
	{
		TA_THREADGUARD(m_storeLock);

		TrainInformation* trainInfo = getTrainInformation(trainID);

		mpuState = trainInfo->activeMPU;
	}
	catch(TrainInvalidTrainException&)
	{
		// do nothing, the method will return MPU_Unknown
	}

	FUNCTION_EXIT;
	return mpuState;
}

std::string TrainStatusManager::printTrainTSIAssignment(TrainID trainID)
{
	FUNCTION_ENTRY("printTrainTSIAssignment");
	std::ostringstream os;
	
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	//	TA_ASSERT(trainInfo->activeMPU == 0 || trainInfo->activeMPU == 1, "Invalid MPU");
	
	os << "[Train TSI Assignment="
		<< "[ActiveMPU=" << static_cast<TrainTypes::MPUState>(trainInfo->activeMPU) << "]"
		<< "[MPUPrimary=" << trainInfo->mpuTSI[0] << "]"
		<< "[MPUSecondary=" << trainInfo->mpuTSI[1] << "]"
		<< "]";
	
	FUNCTION_EXIT;
	return os.str();
}


// get train list and iterate through it, looking at both inactive/active itsi
// but modify the ITSI parameter to be the active entry.
bool TrainStatusManager::isTrain(std::string & ITSI)
{
	FUNCTION_ENTRY("isTrain");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	
    bool found = false;

	if (ITSI != "")
	{
		for (TrainInformationMap::iterator i = m_trainInfoMap.begin(); i != m_trainInfoMap.end(); ++i) 
		{
			if ((*i).second->mpuTSI[TrainTypes::MPU_Primary] == ITSI ||
				(*i).second->mpuTSI[TrainTypes::MPU_Secondary]  == ITSI)
			{
				ITSI = getTrainTSI((*i).second->trainID); // modify inout param
				found = true;
				break;
			}
		}
	}

    FUNCTION_EXIT;
	return found;

}

/**
 * Indicates if the train is mute
 */
bool TrainStatusManager::isTrainMute(unsigned int trainID)
{
	FUNCTION_ENTRY("isTrainMute");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->isMute;
}


/**
 * Get the current location for the train
 */
unsigned long TrainStatusManager::getTrainLocation(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainLocation");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->currentLocation;
}

std::string TrainStatusManager::getTrainServiceNumber(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainServiceNumber");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->serviceNumber;
}

TrainTypes::Track TrainStatusManager::getTrainDirection(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainDirection");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->track;
}
 
bool TrainStatusManager::isTrainLocalisationValid(unsigned int trainID)
{
	FUNCTION_ENTRY("isTrainLocalisationValid");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->isLocalisationValid;
}

/*********************/

namespace {
	/// This method doesn't need a lock, as it relies on caller to have necessary lock
	/// Defined in anonymous namespace to prevent outsiders from using it !!!
	std::string getOwningSessionIDHelper(const TA_IRS_App::TrainStatus::TrainInformation& trainInfo, TA_IRS_App::TrainStatus::ResourceCategory resource) 
	{
		TA_IRS_App::TrainStatus::SessionAllocationMap::const_iterator i = trainInfo.sessionAllocation.find(resource);
		std::string session("");
		if (i != trainInfo.sessionAllocation.end())
		{
			session = (*i).second;    
		}
		return session;
	}

	/// This method doesn't need a lock, as it relies on caller to have necessary lock
	/// Defined in anonymous namespace to prevent outsiders from using it !!!
	unsigned long getOwningCallIDHelper(const TA_IRS_App::TrainStatus::TrainInformation& trainInfo, TA_IRS_App::TrainStatus::ResourceCategory resource) 
	{
		TA_IRS_App::TrainStatus::CallAllocationMap::const_iterator i = trainInfo.callAllocation.find(resource);
		unsigned long callID = 0;
		if (i != trainInfo.callAllocation.end())
		{
			callID = (*i).second;    
		}
		return callID;
	}
}

std::string TrainStatusManager::getOwningSessionID(unsigned int trainID, ResourceCategory resource) 
{
	FUNCTION_ENTRY("getOwningSessionID");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	std::string session = getOwningSessionIDHelper(*trainInfo, resource);

	FUNCTION_EXIT;
	return session;
}

//TD 15938
//zhou yuan++
bool TrainStatusManager::isOperatorBusy(const std::string& sessionID, TA_IRS_App::TrainStatus::ResourceCategory resource)
{
	FUNCTION_ENTRY("isOperatorBusy");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

	bool result = false;
	TrainInformationMap::iterator it = m_trainInfoMap.begin();
	for(; it != m_trainInfoMap.end() ; ++it)
	{
		TA_IRS_App::TrainStatus::SessionAllocationMap::const_iterator i = (it->second)->sessionAllocation.find(resource);
		if (i != (it->second)->sessionAllocation.end())
		{
			if(sessionID == i->second)
			{
				result = true;
				break;
			}
		}	
	}

	FUNCTION_EXIT;
	return result;
}
//++zhou yuan

bool TrainStatusManager::setOwningSessionID(unsigned int trainID, ResourceCategory resource, const std::string& sessionID, bool force)
{
	FUNCTION_ENTRY("setOwningSessionID");
	bool status = true;
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	SessionAllocationMap::iterator i = trainInfo->sessionAllocation.find(resource);
	if ("" == sessionID && force == true)
	{
		// kill existing record, if it exists
		if (i != trainInfo->sessionAllocation.end())
		{
			trainInfo->sessionAllocation.erase(i);
		}
	}
	else if ("" == sessionID && force == false)
	{
		// don't set empty record
        std::stringstream s;
        s << "Session invalid [<empty>]" << std::ends;
        TA_THROW( TrainInvalidParameterException(s.str()) );

		status = false;
	}
	else if (i == trainInfo->sessionAllocation.end() || force == true)
	{
		trainInfo->sessionAllocation[resource] = sessionID;
	}
	else if (sessionID == (*i).second)
    {
        // self
    }
    else
    {
        std::stringstream s;
        s << "Train in use by another session [" 
            << (*i).second
            << "]" << std::ends;
        TA_THROW( TrainInvalidParameterException(s.str()) );

        status = false;
    }
	FUNCTION_EXIT;
	return status;
}

bool TrainStatusManager::amOwningSessionID(unsigned int trainID, ResourceCategory resource, const std::string& sessionID)
{
	FUNCTION_ENTRY("amOwningSessionID");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	SessionAllocationMap::iterator i = trainInfo->sessionAllocation.find(resource);
	bool result = ( i != trainInfo->sessionAllocation.end() && sessionID == (*i).second );
	FUNCTION_EXIT;
	return result;
	//	return "" != sessionID && sessionID == trainInfo->owningSessionID;    
}

void TrainStatusManager::verifyOwningSessionID(unsigned int trainID, ResourceCategory resource, const std::string& sessionID)
{
	FUNCTION_ENTRY("verifyOwningSessionID");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	SessionAllocationMap::iterator i = trainInfo->sessionAllocation.find(resource);
	if (i == trainInfo->sessionAllocation.end())
	{
		return; // no existing session
	}
	else if (sessionID != (*i).second) 
    {
        std::stringstream s;
        s << "Train in use by another session [" 
            << (*i).second
            << "]" << std::ends;
        TA_THROW( TrainInvalidParameterException(s.str()) );
    }
	FUNCTION_EXIT;
}


void TrainStatusManager::setCallAllocation(unsigned int trainID, ResourceCategory resource, unsigned long callId)
{
	FUNCTION_ENTRY("setCallAllocation");

	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);

	// if it is found, and the new ID is 0, then remove the record
	trainInfo->callAllocation[resource] = callId;

	FUNCTION_EXIT;
}


unsigned long TrainStatusManager::getCallAllocation(unsigned int trainID, ResourceCategory resource)
{
	FUNCTION_ENTRY("getCallAllocation");

	unsigned long callId = 0;

	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);

	CallAllocationMap::iterator i = trainInfo->callAllocation.find(resource);
	if (i != trainInfo->callAllocation.end())
	{
		callId = (*i).second;
	}

	FUNCTION_EXIT;
	return callId;
}


void TrainStatusManager::getCallParticipants(unsigned long callId,
                                             std::vector<unsigned char>& trains,
                                             TrainStatus::ResourceCategory& resource)
{
    FUNCTION_ENTRY("getCallParticipants");

    for (TrainInformationMap::const_iterator trainIt = m_trainInfoMap.begin();
    trainIt != m_trainInfoMap.end(); ++trainIt)
    {
        // check the call allocation
        for (CallAllocationMap::const_iterator callIt = trainIt->second->callAllocation.begin();
        callIt != trainIt->second->callAllocation.end(); ++callIt)
        {
            // if the entry matches
            if (callIt->second == callId)
            {
                // add the train to the list
                trains.push_back(trainIt->first);

                // set the resource (its ok to set this multiple times
                // because all calls with this ID will be for the same resource
                resource = callIt->first;

                // check the next train
                break;
            }
        }
    }
	

	FUNCTION_EXIT;
}


void TrainStatusManager::setCallDriverType(unsigned int trainID, TrainStatus::ECallDriverType callType)
{
    FUNCTION_ENTRY("setCallDriverType");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	trainInfo->callDriverType = callType;
	FUNCTION_EXIT;
}


TrainStatus::ECallDriverType TrainStatusManager::getCallDriverType(unsigned int trainID)
{
    FUNCTION_ENTRY("getCallDriverType");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->callDriverType;
}


void TrainStatusManager::addCallToWatchList(unsigned long callId)
{
    TA_ASSERT(callId != 0, "Invalid call Id");

    TA_Base_Core::ThreadGuard guard(m_callLock);

    std::list<unsigned long>::iterator findIter =
        std::find(m_callIdWatchList.begin(), m_callIdWatchList.end(), callId);

    if (findIter == m_callIdWatchList.end())
    {
        m_callIdWatchList.push_back(callId);
    }

    // send a message
    m_notifier.sendSyncMessage(m_callIdWatchList);
}

void TrainStatusManager::removeCallFromWatchList(unsigned long callId)
{
    TA_ASSERT(callId != 0, "Invalid call Id");

    TA_Base_Core::ThreadGuard guard(m_callLock);

    std::list<unsigned long>::iterator findIter =
        std::find(m_callIdWatchList.begin(), m_callIdWatchList.end(), callId);

    if (findIter != m_callIdWatchList.end())
    {
        m_callIdWatchList.erase(findIter);
    }

    // send a message
    m_notifier.sendSyncMessage(m_callIdWatchList);
}

bool TrainStatusManager::isCallInWatchList(unsigned long callId)
{
    TA_Base_Core::ThreadGuard guard(m_callLock);

    std::list<unsigned long>::iterator findIter =
        std::find(m_callIdWatchList.begin(), m_callIdWatchList.end(), callId);

    return (findIter != m_callIdWatchList.end());
}

void TrainStatusManager::synchroniseWatchList(const std::list<unsigned long>& newCallIdWatchList)
{
    TA_Base_Core::ThreadGuard guard(m_callLock);

    m_callIdWatchList = newCallIdWatchList;
}

std::list<unsigned long> TrainStatusManager::getCallWatchList()
{
    TA_Base_Core::ThreadGuard guard(m_callLock);

    return m_callIdWatchList;
}


/*********************/


struct PECPoint TrainStatusManager::getConnectedPECPoint(unsigned int trainID)
{
	FUNCTION_ENTRY("getConnectedPECPoint");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->connectedPECPoint;    
}

TrainStatus::FullPECPointCollection TrainStatusManager::getActivePECPoints()
{
	FUNCTION_ENTRY("getActivePECPoints");

	// get list of all trains
    TrainList trains = getTrainList(true); // We want *ALL* trains we know about, just in case they have an active PEC

	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	
    FullPECPointCollection allPoints;


    // add to the collection
    for (TrainList::iterator iterTrain = trains.begin(); iterTrain != trains.end(); ++iterTrain) 
    {
        TrainInformation* trainInfo = getTrainInformation(*iterTrain);
        // have to do this the hard way, because we need extra info in our return struct
        //        std::copy(trainInfo->activePECPoints.begin(), trainInfo->activePECPoints.end(), std::back_inserter(allPoints));
        for (PECPointCollection::iterator iterPoint = trainInfo->activePECPoints.begin(); iterPoint != trainInfo->activePECPoints.end(); ++iterPoint)
        {
            FullPECPoint point;
            point.trainID = trainInfo->trainID;
            point.serviceNumber = trainInfo->serviceNumber;
			point.track = trainInfo->track;
            point.point = *iterPoint;
//            point.point.car = (*iterPoint).car;
//            point.point.PEC = (*iterPoint).PEC;
//            point.point.source = (*iterPoint).source;
			point.sessionID = getOwningSessionID(trainInfo->trainID, TrainStatus::Resource_PEC);
            allPoints.push_back(point);
        }
    }
    FUNCTION_EXIT;

    return allPoints;
}

/**
 * Gets the PEC point list
 */
//std::vector<TrainStatusManager::PECPoint> TrainStatusManager::getActivePECPoints(unsigned int trainID)
PECPointCollection TrainStatusManager::getActivePECPoints(unsigned int trainID)
{
	FUNCTION_ENTRY("getActivePECPoints");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	FUNCTION_EXIT;
	return trainInfo->activePECPoints;
}


/**
 * Sets the specified PEC point
 *
 * Add to list of PEC points for the train, if no current pec and the new point status 
 * is either Driver or Operator, then copy the point to connectedPECPoint field. 
 */
PECPoint TrainStatusManager::setActivePECPoint(unsigned int trainID, const PECPoint& point, bool setCreationTime)
{
	FUNCTION_ENTRY("setActivePECPoint");
	PECPoint updatedPECPoint;

    // Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

    TrainInformation* trainInfo = getTrainInformation(trainID);
   
    // watch for duplicates - update if we find it is already there
    PECPointCollection::iterator i = std::find_if(trainInfo->activePECPoints.begin(), trainInfo->activePECPoints.end(), 
        std::bind2nd(comparePECPoint(), point));
    if (i != trainInfo->activePECPoints.end())
    {
		// already there, so update only potentially differing fields
        (*i).source = point.source; 
        (*i).validity = point.validity;
		// do not overwrite creationTime, unless this is a second PECActivated message
		if (true == setCreationTime) 
		{
			(*i).creationTime = point.creationTime;
		}
		updatedPECPoint = (*i);
    }
    else 
    {
        trainInfo->activePECPoints.push_back(point); // not there, so add it
		updatedPECPoint = point;
    }

    // if no connected point, and we already know if this one is Selected, make it connected
    if (comparePECPoint()(emptyPoint, trainInfo->connectedPECPoint) && TrainTypes::PEC_UNASSIGNED != point.source) 
    {
        trainInfo->connectedPECPoint = point;
    }

	// Return the updated/created PECPoint
	return updatedPECPoint;

    FUNCTION_EXIT;
}

/**
 * Remove connected PEC point
 *
 * Makes the connectedPECPoint 'unconnected'. It is not removed from the list.
 */
void TrainStatusManager::removeActivePECPoint(unsigned int trainID, const PECPoint& point)
{
	FUNCTION_ENTRY("removeActivePECPoint");

    // Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

    TrainInformation* trainInfo = getTrainInformation(trainID);

    // look for point in list and update its source
    PECPointCollection::iterator i = std::find_if(trainInfo->activePECPoints.begin(), trainInfo->activePECPoints.end(), 
        std::bind2nd(comparePECPoint(), trainInfo->connectedPECPoint));
    if (i != trainInfo->activePECPoints.end())
    {
        (*i).source = point.source;
    }
    // make connected point inactive
    trainInfo->connectedPECPoint = TrainStatusManager::emptyPoint;

    FUNCTION_EXIT;
}


/**
 * Remove an active PEC point from the list
 *
 * Remove point from list of PEC Points for the train.
 * 
 */
void TrainStatusManager::deleteActivePECPoint(unsigned int trainID, const PECPoint& point)
{
	FUNCTION_ENTRY("removeActivePECPoint");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
 
    // check if it is the connected PEC point
    if (comparePECPoint()(trainInfo->connectedPECPoint, point))
    {
        trainInfo->connectedPECPoint = emptyPoint;
    }

    // now look through the list of active points and remove it if it is there
    PECPointCollection::iterator i = std::find_if(trainInfo->activePECPoints.begin(), trainInfo->activePECPoints.end(), 
        std::bind2nd(comparePECPoint(), point));
    if (i != trainInfo->activePECPoints.end())
    {
        trainInfo->activePECPoints.erase(i);
    }
    
    FUNCTION_EXIT;
}

void TrainStatusManager::setConnectedPECPoint(unsigned int trainID, const PECPoint& connectedPoint)
{
	FUNCTION_ENTRY("setConnectedPECPoint");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);

	trainInfo->connectedPECPoint = connectedPoint;

    FUNCTION_EXIT;
}

void TA_IRS_App::TrainStatusManager::resetContinueCounter(unsigned int trainID, ResourceCategory resource)
{
	FUNCTION_ENTRY("resetContinueCounter");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);

	std::ostringstream s;
	s << "TIP5 ContinueCounter " << resource << " [reset]"; 
  
	// doesn't matter if record exists or not, this ensures it is set.
	trainInfo->autoContinueAck[resource] = 0;
	s << " orig value forced to 0";
	s << " new value (" << static_cast<unsigned int>(trainInfo->autoContinueAck[resource]) << ")";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

    FUNCTION_EXIT;
}


void TA_IRS_App::TrainStatusManager::incrementContinueCounter(unsigned int trainID, ResourceCategory resource)
{
	FUNCTION_ENTRY("incrementContinueCounter");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);

	std::ostringstream s;
	s << "TIP5 ContinueCounter " << resource << " [increment]"; 
	// sanity check, protect against continue methods being called in wrong order by clumsy programmer
	if (trainInfo->autoContinueAck.find(resource) == trainInfo->autoContinueAck.end())
	{
		trainInfo->autoContinueAck[resource] = 0;
		s << " orig value forced to 0";
	}
	else
	{
		s << " orig value (" << static_cast<unsigned int>(trainInfo->autoContinueAck[resource]) << ")";
	}

    // Do the check explicitly because exceedContinueLimit locks again...
    if (trainInfo->autoContinueAck[resource] <= m_autoContinueAckLimit) 
    {
        trainInfo->autoContinueAck[resource]++;
    }
	s << " new value (" << static_cast<unsigned int>(trainInfo->autoContinueAck[resource]) << ")";
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

    FUNCTION_EXIT;
}


bool TA_IRS_App::TrainStatusManager::exceededContinueLimit(unsigned int trainID, ResourceCategory resource)
{
	FUNCTION_ENTRY("exceededContinueLimit");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);

	std::ostringstream s;
	s << "TIP5 ContinueCounter " << resource << " [comparison]"; 
	// sanity check, protect against continue methods being called in wrong order by clumsy programmer
	if (trainInfo->autoContinueAck.find(resource) == trainInfo->autoContinueAck.end())
	{
		trainInfo->autoContinueAck[resource] = 0;
		s << " orig value forced to 0";
	}
	else
	{
		s << " orig value (" << static_cast<unsigned int>(trainInfo->autoContinueAck[resource]) << ")";
	}

	bool status = trainInfo->autoContinueAck[resource] > m_autoContinueAckLimit;
	s << " comparison (" << static_cast<unsigned int>(trainInfo->autoContinueAck[resource]) << ")"
		" < (" << m_autoContinueAckLimit << ") = (" << status << ")"; 
	s << std::ends;
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
    
	FUNCTION_EXIT;
    return status;
}




unsigned int TA_IRS_App::TrainStatusManager::getPALibraryVersion(unsigned int trainID)
{
	FUNCTION_ENTRY("getPALibraryVersion");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->PALibraryVersion;
}

void TA_IRS_App::TrainStatusManager::setPALibraryVersion(unsigned int trainID, unsigned int version)
{
	FUNCTION_ENTRY("setPALibraryVersion");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    trainInfo->PALibraryVersion = version;
    FUNCTION_EXIT;
}

std::vector<TA_IRS_App::TrainStatus::TrainVersionInfo> TA_IRS_App::TrainStatusManager::getPALibraryVersion()
{
	FUNCTION_ENTRY("getPALibraryVersion");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

    std::vector<TrainVersionInfo> theList;
    for (TrainInformationMap::iterator i = m_trainInfoMap.begin(); i != m_trainInfoMap.end(); ++i)
    {
        theList.push_back( TrainVersionInfo((*i).first,(*i).second->PALibraryVersion) );
    }


    FUNCTION_EXIT;
	return theList;
}

unsigned char TA_IRS_App::TrainStatusManager::getPAAnnouncementID(unsigned int trainID)
{
	FUNCTION_ENTRY("getPAAnnouncementID");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->PAAnnouncementID;
}

void TA_IRS_App::TrainStatusManager::setPAAnnouncementID(unsigned int trainID, unsigned char announcementID)
{
	FUNCTION_ENTRY("setPAAnnouncementID");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getOrAddTrainInformation(trainID);
    
    trainInfo->PAAnnouncementID = announcementID;
    FUNCTION_EXIT;
}

std::vector<unsigned char> TA_IRS_App::TrainStatusManager::getTrainsForAnnouncement(unsigned char announcementID)
{
    FUNCTION_ENTRY("getTrainsForAnnouncement");

    std::vector<unsigned char> trains;

    for (TrainInformationMap::const_iterator trainIt = m_trainInfoMap.begin();
    trainIt != m_trainInfoMap.end(); ++trainIt)
    {
        // check the announcment ID
        if (trainIt->second->PAAnnouncementID == announcementID)
        {
            // add the train to the list
            trains.push_back(trainIt->first);
        }
    }
	
	FUNCTION_EXIT;
    return trains;
}

unsigned long TA_IRS_App::TrainStatusManager::getCallForAnnouncement(unsigned char announcementID)
{
    FUNCTION_ENTRY("getCallForAnnouncement");
    
    unsigned long callId = 0;

    // check each train until they are all checked or the callId is found
    for (TrainInformationMap::const_iterator trainIt = m_trainInfoMap.begin();
    (callId == 0 ) && ( trainIt != m_trainInfoMap.end() ); ++trainIt)
    {
        // check the announcement ID
        if (trainIt->second->PAAnnouncementID == announcementID)
        {
            // check the call ID
            CallAllocationMap::const_iterator callIt =
                trainIt->second->callAllocation.find(TA_IRS_App::TrainStatus::Resource_PA);
	        if (callIt != trainIt->second->callAllocation.end())
	        {
		        callId = callIt->second;
	        }
        }
    }
	
	FUNCTION_EXIT;
    return callId;
}

unsigned int TA_IRS_App::TrainStatusManager::getTTISLibraryVersion(unsigned int trainID)
{
	FUNCTION_ENTRY("getTTISLibraryVersion");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->TTISLibraryVersion;
}

void TA_IRS_App::TrainStatusManager::setTTISLibraryVersion(unsigned int trainID, unsigned int version)
{
	FUNCTION_ENTRY("setTTISLibraryVersion");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getOrAddTrainInformation(trainID);
    
    trainInfo->TTISLibraryVersion = version;
    FUNCTION_EXIT;
}

unsigned int TA_IRS_App::TrainStatusManager::getTTISScheduleVersion(unsigned int trainID)
{
	FUNCTION_ENTRY("getTTISScheduleVersion");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->TTISScheduleVersion;
}

void TA_IRS_App::TrainStatusManager::setTTISScheduleVersion(unsigned int trainID, unsigned int version)
{
	FUNCTION_ENTRY("setTTISScheduleVersion");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    trainInfo->TTISScheduleVersion = version;
    FUNCTION_EXIT;
}

TA_IRS_App::TrainTypes::TTISStatus TA_IRS_App::TrainStatusManager::getTTISStatus(unsigned int trainID)
{
	FUNCTION_ENTRY("getTTISStatus");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->TTISStatus;
}

void TA_IRS_App::TrainStatusManager::setTTISStatus(unsigned int trainID, TA_IRS_App::TrainTypes::TTISStatus status)
{
	FUNCTION_ENTRY("setTTISStatus");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    trainInfo->TTISStatus = status;
    FUNCTION_EXIT;
}


void TA_IRS_App::TrainStatusManager::setDownloadLibrary(unsigned int trainID, const TA_IRS_App::TTDFrame::TtisFrameList & lib)
{
	FUNCTION_ENTRY("setDownloadLibrary");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    trainInfo->downloadLibrary = lib;
    trainInfo->downloadFrame = 0;
//    trainInfo->downloadPosition = 0;

    FUNCTION_EXIT;

}


TA_IRS_App::TTDFrame::TtisFrameList TA_IRS_App::TrainStatusManager::getDownloadLibrary(unsigned int trainID)
{
	FUNCTION_ENTRY("setDownloadLibrary");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->downloadLibrary;
}

double TA_IRS_App::TrainStatusManager::getTrainCctvZone(TrainID trainID)
{
	FUNCTION_ENTRY("getTrainCctvZone");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->currentCctvZone;
}

bool TA_IRS_App::TrainStatusManager::isCctvActivated(TrainID trainID)
{
	FUNCTION_ENTRY("isCctvActivated");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->cctvActivated;
}

bool TA_IRS_App::TrainStatusManager::isCctvInitialised(TrainID trainID)
{
	FUNCTION_ENTRY("isCctvInitialised");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->cctvInitialised;
}

bool TA_IRS_App::TrainStatusManager::isPreConflict(TrainID trainID)
{
	FUNCTION_ENTRY("isPreConflict");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return (trainInfo->cctvPreConflict>0);
}

TrainID TA_IRS_App::TrainStatusManager::getPreConflict(TrainID trainID)
{
	FUNCTION_ENTRY("isPreConflict");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    FUNCTION_EXIT;
    return trainInfo->cctvPreConflict;
}

void TA_IRS_App::TrainStatusManager::setCctvActivated(TrainID trainID, bool activated)
{
	FUNCTION_ENTRY("setCctvActivated");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getOrAddTrainInformation(trainID);
    
	trainInfo->cctvActivated=activated;
    FUNCTION_EXIT;
}

void TA_IRS_App::TrainStatusManager::setPreConflict(TrainID trainID, TrainID owningTrain)
{
	FUNCTION_ENTRY("setPreConflict");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
	trainInfo->cctvPreConflict=owningTrain;
    FUNCTION_EXIT;
}

std::string TA_IRS_App::TrainStatusManager::getPreConflictAlarmId(TrainID trainID)
{
	FUNCTION_ENTRY("getPreConflictAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
	FUNCTION_EXIT;
	return trainInfo->preConflictAlarmId;
}
void TA_IRS_App::TrainStatusManager::setPreConflictAlarmId(TrainID trainID, std::string alarmId)
{
	FUNCTION_ENTRY("setPreConflictAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
	trainInfo->preConflictAlarmId=alarmId;
    FUNCTION_EXIT;
}
			

void TA_IRS_App::TrainStatusManager::setCctvSwitchState(TrainID trainID, TrainStatus::CctvSwitchState state)
{
	FUNCTION_ENTRY("setCctvSwitchState");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
	trainInfo->cctvSwitchState = state;
	FUNCTION_EXIT;
}	
		
TrainStatus::CctvSwitchState TA_IRS_App::TrainStatusManager::getCctvSwitchState(TrainID trainID)
{
	FUNCTION_ENTRY("getCctvSwitchState");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
	return trainInfo->cctvSwitchState;
	FUNCTION_EXIT;
}	

void TA_IRS_App::TrainStatusManager::defaultTrainCCTVSettings(TrainID trainID)
{
	FUNCTION_ENTRY("defaultTrainCCTVSettings");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);
	trainInfo->cctvInitialised = true;

	m_notifier.defaultTrainCCTV(trainID);
	FUNCTION_EXIT;
}
		
void TA_IRS_App::TrainStatusManager::setCctvInitialised(TrainID trainID, bool initialised)
{
	FUNCTION_ENTRY("setCctvActivated");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
	trainInfo->cctvInitialised=initialised;
    FUNCTION_EXIT;
}

long TA_IRS_App::TrainStatusManager::clearAtcTimerItem(unsigned int trainId)
{
	FUNCTION_ENTRY("getAtcTimerItem");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);

	if (!trainInfo->atcTimerScheduled)
	{
		TA_THROW(TrainGeneralFailureException("AtcTimerItem not scheduled"));
	}
	
	trainInfo->atcTimerScheduled = false;
    
	FUNCTION_EXIT;

	return trainInfo->atcTimerItem;
}

void TA_IRS_App::TrainStatusManager::setAtcTimerItem(unsigned int trainId, long timerItem)
{
	FUNCTION_ENTRY("setAtcTimerItem");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	if (!trainInfo->atcTimerScheduled)
	{
		trainInfo->atcTimerItem = timerItem;
		trainInfo->atcTimerScheduled = true;
	}
	else
	{
		TA_THROW(TrainGeneralFailureException("AtcTimerItem already scheduled"));
	}

	FUNCTION_EXIT;
}

std::string TA_IRS_App::TrainStatusManager::getAtcAlarmId(unsigned int trainId)
{
	FUNCTION_ENTRY("getAtcAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	FUNCTION_EXIT;

	return trainInfo->atcAlarmId;
}

void TA_IRS_App::TrainStatusManager::setAtcAlarmId(unsigned int trainId, std::string alarmId)
{
	FUNCTION_ENTRY("setAtcAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getOrAddTrainInformation(trainId);
    
	if (trainInfo->atcAlarmId == "")
	{
		trainInfo->atcAlarmId = alarmId;
	}
	else
	{
		TA_THROW(TrainGeneralFailureException("AtcAlarmId already set"));
	}

	FUNCTION_EXIT;
}

void TA_IRS_App::TrainStatusManager::resetAtcAlarmId(unsigned int trainId, std::string alarmId)
{
	FUNCTION_ENTRY("resetAtcAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	trainInfo->atcAlarmId = alarmId;

	FUNCTION_EXIT;
}

bool TA_IRS_App::TrainStatusManager::noAtcAlarms()
{
	FUNCTION_ENTRY("noAtcAlarms");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    
	TrainInformationMap::iterator it;
	
	for (it = m_trainInfoMap.begin(); it != m_trainInfoMap.end(); it++)
	{
		if ((*it).second->atcAlarmId != "")
		{
			FUNCTION_EXIT
			return false;
		}
	}
   
	FUNCTION_EXIT;

	return true;
}


long TA_IRS_App::TrainStatusManager::clearOa1TimerItem(unsigned int trainId)
{
	FUNCTION_ENTRY("getOa1TimerItem");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	if (!trainInfo->oa1TimerScheduled)
	{
		TA_THROW(TrainGeneralFailureException("Oa1TimerItem not scheduled"));
	}
	
	trainInfo->oa1TimerScheduled = false;
    
	FUNCTION_EXIT;

	return trainInfo->oa1TimerItem;
}

void TA_IRS_App::TrainStatusManager::setOa1TimerItem(unsigned int trainId, long timerItem)
{
	FUNCTION_ENTRY("setOa1TimerItem");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	if (!trainInfo->oa1TimerScheduled)
	{
		trainInfo->oa1TimerItem = timerItem;
		trainInfo->oa1TimerScheduled = true;
	}
	else
	{
		TA_THROW(TrainGeneralFailureException("Oa1TimerItem already scheduled"));
	}

	FUNCTION_EXIT;
}

std::string TA_IRS_App::TrainStatusManager::getOa1AlarmId(unsigned int trainId)
{
	FUNCTION_ENTRY("getOa1AlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	FUNCTION_EXIT;

	return trainInfo->oa1AlarmId;
}

void TA_IRS_App::TrainStatusManager::setOa1AlarmId(unsigned int trainId, std::string alarmId)
{
	FUNCTION_ENTRY("setOa1AlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	if (trainInfo->oa1AlarmId == "")
	{
		trainInfo->oa1AlarmId = alarmId;
	}
	else
	{
		TA_THROW(TrainGeneralFailureException("Oa1AlarmId already set"));
	}

	FUNCTION_EXIT;
}

void TA_IRS_App::TrainStatusManager::resetOa1AlarmId(unsigned int trainId, std::string alarmId)
{
	FUNCTION_ENTRY("resetOa1AlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	trainInfo->oa1AlarmId = alarmId;

	FUNCTION_EXIT;
}

bool TA_IRS_App::TrainStatusManager::noOa1Alarms()
{
	FUNCTION_ENTRY("noAtcAlarms");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    
	TrainInformationMap::iterator it;
	
	for (it = m_trainInfoMap.begin(); it != m_trainInfoMap.end(); it++)
	{
		if ((*it).second->atcAlarmId != "")
		{
			FUNCTION_EXIT
			return false;
		}
	}
   
	FUNCTION_EXIT;

	return true;
}

std::string TA_IRS_App::TrainStatusManager::getRadioModeAlarmId(unsigned int trainId)
{
    FUNCTION_ENTRY("getRadioModeAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	FUNCTION_EXIT;

	return trainInfo->radioModeAlarmID;
}

void TA_IRS_App::TrainStatusManager::setRadioModeAlarmId(unsigned int trainId, std::string alarmId)
{
	FUNCTION_ENTRY("setRadioModeAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	if (trainInfo->radioModeAlarmID == "")
	{
		trainInfo->radioModeAlarmID = alarmId;
	}
	else
	{
		TA_THROW(TrainGeneralFailureException("RadioModeAlarmId already set"));
	}

	FUNCTION_EXIT;
}

void TA_IRS_App::TrainStatusManager::resetRadioModeAlarmId(unsigned int trainId, std::string alarmId)
{
	FUNCTION_ENTRY("resetRadioModeAlarmId");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainId);
    
	trainInfo->radioModeAlarmID = alarmId;

	FUNCTION_EXIT;
}

bool TA_IRS_App::TrainStatusManager::noRadioModeAlarms()
{
	FUNCTION_ENTRY("noRadioModeAlarms");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    
	TrainInformationMap::iterator it;
	
	for (it = m_trainInfoMap.begin(); it != m_trainInfoMap.end(); it++)
	{
		if ((*it).second->radioModeAlarmID != "")
		{
			FUNCTION_EXIT
			return false;
		}
	}
   
	FUNCTION_EXIT;

	return true;
}
		
void TA_IRS_App::TrainStatusManager::setResendFrames(unsigned int trainID, const std::vector<unsigned int> & frames)
{
	FUNCTION_ENTRY("setDownloadLibrary");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    trainInfo->resendFrames = frames;
    trainInfo->downloadFrame = 0;
//    trainInfo->downloadPosition = 0;

    FUNCTION_EXIT;

}

std::vector<std::string> TA_IRS_App::TrainStatusManager::getTrainSelectionList()
{
	FUNCTION_ENTRY("getTrainSelectionList");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

    std::vector<std::string> theList;
    for (TrainSelectionMap::iterator i = m_trainSelectionMap.begin(); i != m_trainSelectionMap.end(); ++i)
    {
        theList.push_back((*i).first);
    }
    
    FUNCTION_EXIT;
    return theList;
}

TA_IRS_App::TrainList TA_IRS_App::TrainStatusManager::getTrainSelection(const std::string & selectionName)
{
	FUNCTION_ENTRY("getTrainSelection");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

	TrainList theList;

    TrainSelectionMap::iterator i = m_trainSelectionMap.find(selectionName);
    if (i == m_trainSelectionMap.end())
    {
        FUNCTION_EXIT;
        TA_THROW(TrainInvalidSelectionListException());
    }

    FUNCTION_EXIT;
    return (*i).second;    
}

bool TA_IRS_App::TrainStatusManager::addTrainSelection(const std::string & selectionName, const TA_IRS_App::TrainList& trainList)
{
	FUNCTION_ENTRY("addTrainSelection");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

	bool isAdded = (m_trainSelectionMap.find(selectionName)==m_trainSelectionMap.end());
	m_trainSelectionMap[selectionName] = trainList;

    FUNCTION_EXIT;
	return isAdded;
}

void TA_IRS_App::TrainStatusManager::delTrainSelection(const std::string & selectionName)
{
	FUNCTION_ENTRY("addTrainSelection");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);

    TrainSelectionMap::iterator i = m_trainSelectionMap.find(selectionName);
    if (i == m_trainSelectionMap.end())
    {
        FUNCTION_EXIT;
        TA_THROW(TrainInvalidSelectionListException());
    }

    m_trainSelectionMap.erase(i);

    FUNCTION_EXIT;
}

/**
* @todo Consider using a stragety pattern to better control when updates
* are actually performed.
*/
void TA_IRS_App::TrainStatusManager::onStateChange(TrainID trainID, enum TrainStatus::SyncState syncState)
{
	FUNCTION_ENTRY("onStateChange");
	TA_THREADGUARD(m_storeLock);

	if(syncState == syncOperationMode)
	{
		m_notifier.sendOperationModeSyncMessage();
	}
	else
	{
		std::vector<TrainStatus::TrainInformation> changedList;
		/// @todo look through list to see if any additional changes to send
		/// @todo consider modifying to use strategy pattern to abstract send trigger. 
		TrainInformation* trainInfo = getTrainInformation(trainID);

		trainInfo->syncState = syncState;
		changedList.push_back(*trainInfo);
	
		m_notifier.sendSyncMessage(changedList);
	}    
	FUNCTION_EXIT;
}

std::string TA_IRS_App::TrainStatusManager::getResourceSessionID(TrainID trainID, const TrainInformation& trainInfo, TA_IRS_App::TrainStatus::ResourceCategory resource)
{
	std::string sessionID = getOwningSessionIDHelper(trainInfo, resource);
	if(sessionID.size() != 0)
	{
		setOwningSessionID(trainID, resource, sessionID);
	}
	return sessionID;
}


unsigned long TA_IRS_App::TrainStatusManager::getResourceCallID(TrainID trainID, const TrainInformation& trainInfo, TA_IRS_App::TrainStatus::ResourceCategory resource)
{
	unsigned long callID = getOwningCallIDHelper(trainInfo, resource);
	if(callID != 0)
	{
		setCallAllocation(trainID, resource, callID);
	}
	return callID;
}

unsigned long TA_IRS_App::TrainStatusManager::getCctvStageId(double cctvZone)
{
	if(cctvZone < 0)
	{
		cctvZone *= -1;
	}
	unsigned long stage = 0;
	
	std::stringstream stageExtract;
	stageExtract << cctvZone;

	stageExtract >> stage;
	
	return stage;
}

void TA_IRS_App::TrainStatusManager::onTrainStateUpdate(std::vector<TrainStatus::TrainInformation>& updateList)
{
	FUNCTION_ENTRY("onTrainStateUpdate");
	std::stringstream s;
	bool update = false;

	for (std::vector<TrainStatus::TrainInformation>::const_iterator i = 
		updateList.begin(); i != updateList.end(); ++i)
	{
		if(i->syncState != syncNOP)
		{
			s.clear();
			s << "sync message in trainID: [" << static_cast<int>(i->trainID) << "] "
				<< "Sync: [" << i->syncState << "] ";
			switch(i->syncState)
			{
				case TrainStatus::syncCallDriver:
				{
					try
					{
						TA_THREADGUARD(m_storeLock);
						std::string sessionID = getResourceSessionID(i->trainID, *i, Resource_CallDriver);
						unsigned long callID = getResourceCallID(i->trainID, *i, Resource_CallDriver);
						TrainInformation* trainInfo = getOrAddTrainInformation(i->trainID);
						trainInfo->trainStatus = i->trainStatus;
						setOwningSessionID(i->trainID, Resource_CallDriver, sessionID, true);
						setCallAllocation(i->trainID, Resource_CallDriver, callID);
						s << "trainStatus: [" << trainInfo->trainStatus << "] "
							<< "sessionID: [" << sessionID << "] "
							<< "callID: [" << callID << "] complete" << std::ends;
						update = true;
					}
					catch(...)
					{
						update = false;
					}
					break;
				}
				case TrainStatus::syncCctv:
				{
					try
					{
						setCctvActivated(i->trainID, i->cctvActivated);
						setCctvInitialised(i->trainID, i->cctvInitialised);
						setPreConflict(i->trainID, i->cctvPreConflict);
						setPreConflictAlarmId(i->trainID, i->preConflictAlarmId);
						setCctvSwitchState(i->trainID, i->cctvSwitchState);
						std::string sessionID = getResourceSessionID(i->trainID, *i, Resource_CCTV);
						unsigned long callID = getResourceCallID(i->trainID, *i, Resource_CCTV);
						setOwningSessionID(i->trainID, Resource_CCTV, sessionID, true);
						setCallAllocation(i->trainID, Resource_CCTV, callID);
						s << "cctvActivated: [" << isCctvActivated(i->trainID) << "] "
							<< "cctvInitialised: [" << isCctvInitialised(i->trainID) << "] "
							<< "cctvPreConflict: [" << static_cast<unsigned int>(getPreConflict(i->trainID)) << "] "
							<< "cctvSwitchState: [" << getCctvSwitchState(i->trainID) << "] "
							<< "sessionID: [" << sessionID << "] "
							<< "callID: [" << callID << "] complete" << std::ends;
						m_notifier.notifyCCTV(i->trainID, isCctvActivated(i->trainID), isPreConflict(i->trainID), getCctvStageId(getTrainCctvZone(i->trainID)), getCctvSwitchState(i->trainID));
						update = true;
    					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Processed Cctv state update for train %i", (int)(i->trainID));
					}
					catch(...)
					{
						update = false;
					}
					break;
				}
				case TrainStatus::syncLibraryVersions:
				{				
					try
					{
						setTTISLibraryVersion(i->trainID, i->TTISLibraryVersion);
						setPALibraryVersion(i->trainID, i->PALibraryVersion);
						setTTISScheduleVersion(i->trainID, i->TTISScheduleVersion);
						s << "TTISLibraryVersion: [" << getTTISLibraryVersion(i->trainID) << "] "
							<< "PALibraryVersion: [" << getPALibraryVersion(i->trainID) << "] "
							<< "TTISScheduleVersion: [" << getTTISScheduleVersion(i->trainID) << "] complete"
							<< std::ends;
						m_notifier.notifyPublicAddress(i->trainID, i->PALibraryVersion);
						update = true;
					}
					catch(...)
					{
						update = false;
					}
					break;
				}
				case TrainStatus::syncPassengerEmergencyCall:
				{
					try
					{
						clearActivePECPoints(i->trainID);
						for(PECPointCollection::const_iterator pec = i->activePECPoints.begin();
							pec != i->activePECPoints.end(); ++pec)
						{
							PECPoint updatedPoint;
							updatedPoint = setActivePECPoint(i->trainID, *pec, true);
						}
						std::string sessionID = getResourceSessionID(i->trainID, *i, Resource_PEC);
						unsigned long callID = getResourceCallID(i->trainID, *i, Resource_PEC);
						setOwningSessionID(i->trainID, Resource_PEC, sessionID, true);
						setCallAllocation(i->trainID, Resource_PEC, callID);
						s << "activePECPoints: [" << i->activePECPoints << "]"
							<< "sessionID: [" << sessionID << "] "
							<< "callID: [" << callID << "] complete" << std::ends;
						update = true;
					}
					catch(...)
					{
						update = false;
					}
					break;
				}
				case TrainStatus::syncPublicAddress:
				{
					try
					{
						setPAAnnouncementID(i->trainID, i->PAAnnouncementID);
						std::string sessionID = getResourceSessionID(i->trainID, *i, Resource_PA);
						unsigned long callID = getResourceCallID(i->trainID, *i, Resource_PA);
						setOwningSessionID(i->trainID, Resource_PA, sessionID, true);
						setCallAllocation(i->trainID, Resource_PA, callID);
						s << "PAAnnouncementID: [" << static_cast<unsigned int>(getPAAnnouncementID(i->trainID)) << "] "
							<< "sessionID: [" << sessionID << "] "
							<< "callID: [" << callID << "] complete" << std::ends;
						update = true;
					}
					catch(...)
					{
						update = false;
					}
					break;
				}
				case TrainStatus::syncTSI:
				{
					try
					{
						setTrainTSI(i->trainID, i->mpuTSI[TrainTypes::MPU_Primary], TrainTypes::MPU_Primary, true);
						setTrainTSI(i->trainID, i->mpuTSI[TrainTypes::MPU_Secondary], TrainTypes::MPU_Secondary, true);
						if(i->activeMPU == 0)
						{
							setTrainTSI(i->trainID, i->mpuTSI[i->activeMPU], TrainTypes::MPU_Primary, true);
						}
						else
						{
							setTrainTSI(i->trainID, i->mpuTSI[i->activeMPU], TrainTypes::MPU_Secondary, true);
						}
						s << "mpuTSI: [" << getTrainTSI(i->trainID).c_str() << "] complete" << std::ends;
						update = true;
					}
					catch(...)
					{
						update = false;
					}
					break;
				}
				case TrainStatus::syncAlarmIDs:
				{
					try
					{
						setAtcAlarmId(i->trainID, i->atcAlarmId);
						setOa1AlarmId(i->trainID, i->oa1AlarmId);
                        setRadioModeAlarmId(i->trainID, i->radioModeAlarmID);
						s << "atcAlarmId: [" << i->atcAlarmId << "]"
							<< "atcAlarmId: [" << i->atcAlarmId << "] "
                            << "radioModeAlarmID: [" << i->radioModeAlarmID << "] "<< std::ends;
						update = true;
					}
					catch(...)
					{
						update = false;
					}
					break;
				}
				default:
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Undefined message sync state");
					break;
				}
			} // Switch
			if(update == false)
			{
				s << "Train record does not exist" << std::ends;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, s.str().c_str());
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());
			}
		}
	}
	FUNCTION_EXIT;
}

void TA_IRS_App::TrainStatusManager::setSyncState(TrainID trainID, TrainStatus::SyncState syncState)
{
	FUNCTION_ENTRY("setSyncState");
	// Lock the store while we write to it
	TA_THREADGUARD(m_storeLock);
    TrainInformation* trainInfo = getTrainInformation(trainID);
    
    trainInfo->syncState = syncState;
    FUNCTION_EXIT;

}

void TA_IRS_App::TrainStatusManager::clearActivePECPoints(unsigned int trainID)
{
	TA_THREADGUARD(m_storeLock);

	//if we dont know the train then we need to create it and clear the PEC anyway
	TrainInformation* trainInfo = getOrAddTrainInformation(trainID);

	trainInfo->activePECPoints.clear();
}

TrainTypes::AtsValid TA_IRS_App::TrainStatusManager::getTrainAtsValid(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainAtsValid");

	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	TrainInformation* trainInfo = getTrainInformation(trainID);

	FUNCTION_EXIT;
	return trainInfo->atsValid;
}

unsigned char TA_IRS_App::TrainStatusManager::trainTSI2ID(const std::string & TSI)
{
	FUNCTION_ENTRY("trainTSI2ID");
	// Lock the store while we read it
	TA_THREADGUARD(m_storeLock);
	
    unsigned char found = 0;

	if (TSI != "")
	{
		for (TrainInformationMap::iterator i = m_trainInfoMap.begin(); i != m_trainInfoMap.end(); ++i) 
		{
			if ((*i).second->mpuTSI[TrainTypes::MPU_Primary] == TSI ||
				(*i).second->mpuTSI[TrainTypes::MPU_Secondary]  == TSI)
			{
				found = (*i).second->trainID;
				break;
			}
		}
	}

    FUNCTION_EXIT;
	return found;
}


TrainTypes::TrainStatus TA_IRS_App::TrainStatusManager::getTrainStatus(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainStatus");

	TA_THREADGUARD(m_storeLock);

	TrainInformation* trainInfo = getOrAddTrainInformation(trainID);

	FUNCTION_EXIT;
	return trainInfo->trainStatus;
}


//********************
// Helper functions  *
//********************

// This function will get the details for the train (or throw an exception if it cannot find them)
// Do not thread lock - already done by calling function
TA_IRS_App::TrainStatus::TrainInformation* TrainStatusManager::getTrainInformation(unsigned int trainID)
{
	FUNCTION_ENTRY("getTrainInformation");
	TrainInformationMap::const_iterator   it;

	// Find the train in the map
	it = m_trainInfoMap.find(trainID);
	if (it==m_trainInfoMap.end())
	{
		TA_THROW(TrainInvalidTrainException());
	}

	FUNCTION_EXIT;
	return (*it).second;
}

// Do not thread lock - already done by calling function
TA_IRS_App::TrainStatus::TrainInformation* TrainStatusManager::addTrainInformation(unsigned int trainID)
{
	FUNCTION_ENTRY("addTrain");

	std::ostringstream s;
	s << "addTrain [TrainID=" << static_cast<int>(trainID) << "]";
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
		TA_Base_Core::DebugUtil::DebugInfo, s.str().c_str());

	TrainInformation*		trainInfo;
	TrainInformationMap::const_iterator   it;

	// Create a new information entry
	trainInfo = new TrainInformation;
	TA_ASSERT(trainInfo!=NULL,"Unable to create train information entry.");

	// Initialise the entry
	trainInfo->trainID = trainID;

	trainInfo->currentLocation = 0;
	trainInfo->serviceNumber = "";
	trainInfo->isMute = false;
	trainInfo->track = TrainTypes::Track_Unknown;
    trainInfo->isLocalisationValid = false;
    trainInfo->atsValid = TrainTypes::AtsValid_Unknown;
	
	trainInfo->TTISLibraryVersion = 0;
	trainInfo->TTISScheduleVersion = 0;

//	trainInfo->downloadLibrary = NULL;
	trainInfo->downloadFrame = 0;
//	trainInfo->downloadPosition = 0;

   	trainInfo->PALibraryVersion = 0;
    trainInfo->PAAnnouncementID = 0;

	trainInfo->currentCctvZone = 0;
	trainInfo->cctvInitialised = false;
	trainInfo->cctvActivated = false;
	trainInfo->cctvPreConflict = 0;
	trainInfo->preConflictAlarmId = "";
	trainInfo->cctvSwitchState.mode = seq;
	trainInfo->cctvSwitchState.item1 = 1;
	trainInfo->cctvSwitchState.item2 = 0;
	trainInfo->cctvSwitchState.item3 = 0;
	trainInfo->cctvSwitchState.item4 = 0;


	trainInfo->trainStatus = TrainTypes::IDLE;

	trainInfo->connectedPECPoint = TrainStatusManager::emptyPoint;
    trainInfo->activeMPU = TrainTypes::MPU_Primary;

//    trainInfo->continueCounter = 0;
//    trainInfo->callID = 0;

	// Add it to the map
	it = m_trainInfoMap.find(trainID);
	TA_ASSERT(it==m_trainInfoMap.end(),"Train is already in the map.");
	m_trainInfoMap[trainID] = trainInfo;

	// Return the pointer
	return trainInfo;
}

// This function will get the details for the train (or throw an exception if it cannot find them)
// Do not thread lock - already done by calling function
TA_IRS_App::TrainStatus::TrainInformation* TrainStatusManager::getOrAddTrainInformation(unsigned int trainID)
{
	FUNCTION_ENTRY("getOrAddTrainInformation");
	TrainInformationMap::const_iterator   it;

	TrainInformation* trainInfo = 0;

	// Find the train in the map
	it = m_trainInfoMap.find(trainID);
	if (it==m_trainInfoMap.end())
	{
		trainInfo = addTrainInformation(trainID);
	}
	else
	{
		trainInfo = (*it).second;
	}

	FUNCTION_EXIT;
	return trainInfo;
}

/*******************
**
** TSI Update Strategies
**
*******************/

TA_IRS_App::TrainTSIUpdateStrategy::TrainTSIUpdateStrategy( TA_IRS_App::TrainStatusManager & mgr )
: m_context(mgr)
{
}

void TA_IRS_App::TrainTSIUpdateStrategyAlways::setTSI( TrainEvent & event, const TrainEventReader * reader, bool isSync )
{
	m_context.setTrainTSI(event.getTrainID(), event.getTrainITSI(), convertSDSDataOrigin(event.getOrigin()));
}

void TA_IRS_App::TrainTSIUpdateStrategyStrictWakeup::setTSI( TrainEvent & event, const TrainEventReader * reader, bool isSync )
{
	if (dynamic_cast<const TA_IRS_App::miscTestCallEventReader*>(reader) != 0)
	{
		m_context.setTrainTSI(event.getTrainID(), event.getTrainITSI(), convertSDSDataOrigin(event.getOrigin()));
	}
}

/*******************
**
** Various Helpers
**
*******************/

TA_IRS_App::TrainStateUpdateObserver::TrainStateUpdateObserver(TrainStatusManager & mgr, TrainID train, enum TrainStatus::SyncState syncState)
: m_trainStatusManager(mgr), m_trainID(train), m_syncState(syncState), m_synchronise(true)
{
}

TA_IRS_App::TrainStateUpdateObserver::~TrainStateUpdateObserver()
{
	// To Do:  Enable synchronisation after it is ready for testing
	synchronise();
}

void TA_IRS_App::TrainStateUpdateObserver::synchronise()
{
	if(m_synchronise == true)
	{
		// Ensure this is only done once
		m_synchronise = false;
		m_trainStatusManager.onStateChange(m_trainID, m_syncState);
	}	
}


namespace 
{

	std::string indent(int num)
	{
		std::ostringstream s;
		for (int i = 0; i < num; ++i)
		{
			s << "\t";
		}
		return s.str();
	};

}


std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::TrainInformation & x)
{
	int level = 0;
	os << indent(level++) << "[TrainInformation=" << std::endl;

	os << indent(level) << "[TrainID=" << static_cast<int>(x.trainID) << "]" << std::endl;			

	os << indent(level) << "[SessionAllocationMap=" << x.sessionAllocation << "]" << std::endl;
	os << indent(level) << "[CallAllocationMap=" <<  x.callAllocation << "]" << std::endl;
	os << indent(level) << "[AutoContinueAckMap=" << x.autoContinueAck << "]" <<std::endl;

	os << indent(level) << "[TrainID=" << x.currentLocation << "]" << std::endl;			
	os << indent(level) << "[ServiceNo=" << x.serviceNumber << "]" << std::endl;			
	os << indent(level) << "[IsMute=" << x.isMute << "]" << std::endl;			
	os << indent(level) << "[Track=" << x.track << "]" << std::endl;			
	os << indent(level) << "[IsLocalisationValid=" << x.isLocalisationValid << "]" << std::endl;
	os << indent(level) << "[IsValid=" << x.atsValid << "]" << std::endl;			

	os << indent(level) << "[primaryTSI=" << x.mpuTSI[0] << "]" << std::endl;			
	os << indent(level) << "[secondaryTSI=" << x.mpuTSI[1] << "]" << std::endl;			
	os << indent(level) << "[activeMPU=" << x.activeMPU << "]" << std::endl;			

	os << indent(level) << "[TTISLibraryVersion=" << x.TTISLibraryVersion << "]" << std::endl;			
	os << indent(level) << "[TTISScheduleVersion=" << x.TTISScheduleVersion << "]" << std::endl;			
	os << indent(level) << "[TTISStatus=" << x.TTISStatus << "]" << std::endl;			

	os << indent(level) << "[DownloadLibrary=" << "???" << "]" << std::endl;			
	os << indent(level) << "[DownloadFrame=" << x.downloadFrame << "]" << std::endl;
	
	os << indent(level) << "[resendFrames=";
	os << x.resendFrames.size() << "[";
	{
		for(std::vector<unsigned int>::const_iterator i = x.resendFrames.begin(); i != x.resendFrames.end(); ++i)
		{
			os << *i;
			if((i + 1) != x.resendFrames.end())
			{
				os << ":";
			}
		}
	}
	os<< "]]" << std::endl;

	os << indent(level) << "[PALibraryVersion=" << x.PALibraryVersion << "]" << std::endl;			
	os << indent(level) << "[PAAnnouncementID=" << static_cast<unsigned int>(x.PAAnnouncementID) << "]" << std::endl;			

	os << indent(level) << "[cctvActivated=" << x.cctvActivated << "]" << std::endl;			
	os << indent(level) << "[cctvInitialised=" << x.cctvInitialised << "]" << std::endl;			
	os << indent(level) << "[cctvPreConflict=" << static_cast<unsigned int>(x.cctvPreConflict) << "]" << std::endl;			
	os << indent(level) << "[cctvSwitchState=" << x.cctvSwitchState << "]" << std::endl;			

	os << indent(level) << "[trainStatus=" << x.trainStatus << "]" << std::endl;			

	os << indent(level) << "[activePECPoints=" << x.activePECPoints << "]" << std::endl;
	os << indent(level) << "[connectedPECPoint=" << x.connectedPECPoint << "]" << std::endl;			

	os << indent(level) << "[syncState=" << x.syncState << "]" << std::endl;
	os << indent(--level) << "]" << std::endl;

	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::ResourceCategory& x)
{
	switch(x)
	{
		case Resource_CallDriver:
		{
			os << "Resource_CallDriver";
			break;
		}
		case Resource_CCTV:
		{
			os << "Resource_CCTV";
			break;
		}
		case Resource_PA:
		{
			os << "Resource_PA";
			break;
		}
		case Resource_PEC:
		{
			os << "Resource_PEC";
			break;
		}
		case Resource_TTIS:
		{
			os << "Resource_TTIS";
			break;
		}
		default:
		{
			os << "UNDEFINED ResourceCategory";
			break;
		}
	}
	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::SessionAllocationMap& x)
{
	os << x.size();
	for(SessionAllocationMap::const_iterator r = x.begin(); r != x.end(); ++r)
	{
		os << "[" << r->first << ":" <<  r->second << "]";
	}

	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::CallAllocationMap& x)
{
	os << x.size();
	for(CallAllocationMap::const_iterator r = x.begin(); r != x.end(); ++r)
	{
		os << "[" << r->first << ":" <<  r->second << "]";
	}

	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::AutoContinueAckMap& x)
{
	os << x.size();
	for(AutoContinueAckMap::const_iterator r = x.begin(); r != x.end(); ++r)
	{
		os << "[" << r->first << ":" <<  r->second << "]";
	}

	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::SyncState& x)
{
	switch(x)
	{
		case syncCallDriver:
		{
			os << "syncCallDriver";
			break;
		}
		case syncCctv:
		{
			os << "syncCctv";
			break;
		}
		case syncLibraryVersions:
		{
			os << "syncLibraryVersions";
			break;
		}
		case syncNOP:
		{
			os << "syncNOP";
			break;
		}
		case syncPassengerEmergencyCall:
		{
			os << "syncPassengerEmergencyCall";
			break;
		}
		case syncPublicAddress:
		{
			os << "syncPublicAddress";
			break;
		}
		case syncTSI:
		{
			os << "syncTSI";
			break;
		}
		case syncAlarmIDs:
		{
			os << "syncAlarmIDs";
			break;
		}
		default:
		{
			os << "UNDEFINED SyncState";
			break;
		}
	}

	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::TrainStatus::CctvSwitchState& x)
{
	switch(x.mode)
	{
		case cam:
		{
			os << "cam";
			break;
		}
		case seq:
		{
			os << "seq";
			break;
		}
		case TrainStatus::quad:
		{
			os << "quad";
			break;
		}
		default:
		{
			os << "UNDEFINED CctvSwitchState";
			break;
		}
	}

	return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_App::PECPointCollection& x)
{
	os << x.size();
	for (PECPointCollection::const_iterator i = x.begin(); i != x.end(); ++i)
	{
		os << "[point=" << *i << "]";			
	}

	return os;
}
