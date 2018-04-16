/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/CtaEcsAlarms.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <iostream>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/ECSAgentAlarms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/CtaEcsAlarms.h"


CtaEcsAlarms::CtaEcsAlarms(ItaEcsCachedConfig& cachedConfigInstance, ItaEcsCachedMaps& cachedMapsInstance)
: m_ecsInvalidConfigurationRaised(true),  // Deliberately set to true so that the first time it 
                                          // is raised after startup, it will close any existing
                                          // alarms.
  /*/TD18093,jianghp, m_nonUniqueAlarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper()),
  m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper()),*/
  m_cachedConfigInstance(cachedConfigInstance),
  m_cachedMapsInstance(cachedMapsInstance)
{
	FUNCTION_ENTRY("CtaEcsAlarms");
    m_locationKeyToAlarmIdMap.clear();
	FUNCTION_EXIT;

}

CtaEcsAlarms::~CtaEcsAlarms()
{
	FUNCTION_ENTRY("~CtaEcsAlarms");
    m_locationKeyToAlarmIdMap.clear();

    TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
	FUNCTION_EXIT;
}


// AbstractFailoverable overrides 
void CtaEcsAlarms::setControl(bool isOnStartup)
{   
	FUNCTION_ENTRY("setControl");
    // Important that this is called first the set the enabled flag.
    AbstractFailoverable::setControl(isOnStartup);

    if (isOnStartup)
    {
		// Don't close the EmergencyNormalisationAlarms - operator is to manually close.
		closeEcsInterAgentCommsFailure();
		closeEcsInvalidConfiguration(true);
    }
	FUNCTION_EXIT;
}

// ExceptionChecked
void CtaEcsAlarms::raiseEcsInterAgentCommsFailure( unsigned long locationKey )
{
 	FUNCTION_ENTRY("raiseEcsInterAgentCommsFailure");

  	TA_Base_Core::ThreadGuard guard( m_lock );

    if (getIsEcsInterAgentCommsFailureRaised(locationKey))
    {
        // already raised
		FUNCTION_EXIT;
        return;
    }

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }
        
        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
        TA_Base_Core::NameValuePair pair1 ( "Location", m_cachedMapsInstance.getLocationName(locationKey) );
	    descriptionParameters.push_back ( &pair1 );

        std::string alarmId = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
            TA_Base_Core::ECSAgentAlarms::EcsInterAgentCommsFailure, 
            m_cachedConfigInstance.getAgentKey(),	        	// entity key
            m_cachedConfigInstance.getAgentTypeKey(),	    	// entity type key
            descriptionParameters,                                  // parameters
		    m_cachedConfigInstance.getAgentName(),			// entity name
            m_cachedConfigInstance.getAgentLocationKey(),		// region key
            m_cachedConfigInstance.getAgentSubsystemKey(),	// subystem key
			m_cachedConfigInstance.getAssetName(),
			TA_Base_Core::AlarmConstants::defaultTime );

        m_locationKeyToAlarmIdMap[locationKey] = alarmId;
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to raise the EcsInterAgentCommsFailure alarm");
    }
    
    FUNCTION_EXIT;
}

// ExceptionChecked
void CtaEcsAlarms::closeEcsInterAgentCommsFailure( unsigned long locationKey )
{
 	FUNCTION_ENTRY("closeEcsInterAgentCommsFailure(locationKey)");

  	TA_Base_Core::ThreadGuard guard( m_lock );

    if (!getIsEcsInterAgentCommsFailureRaised(locationKey))
    {
        // not active
        return;
    }

    try
    {   
        TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(
            m_locationKeyToAlarmIdMap[locationKey],
            m_cachedConfigInstance.getAgentLocationKey());

        // Remove this alarm from the list
        m_locationKeyToAlarmIdMap.erase(locationKey);
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to close the EcsInterAgentCommsFailure alarm");
    }
    

    FUNCTION_EXIT;
}

// ExceptionChecked
void CtaEcsAlarms::closeEcsInterAgentCommsFailure()
{
 	FUNCTION_ENTRY("closeEcsInterAgentCommsFailure()");

    TA_Base_Core::ThreadGuard guard( m_lock );

    // Obtain handle to the unique Alarm Helper.  Deliberately using a unique 
    // helper for non-unique alarms to close all alarms of this type.

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
            TA_Base_Core::ECSAgentAlarms::EcsInterAgentCommsFailure, 
            m_cachedConfigInstance.getAgentKey(),
            m_cachedConfigInstance.getAgentLocationKey());

        m_locationKeyToAlarmIdMap.clear();
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to close the EcsInterAgentCommsFailure alarms");
    }
    

    FUNCTION_EXIT;
}




void CtaEcsAlarms::raiseEcsInvalidConfiguration( const std::string& detail )
{
 	FUNCTION_ENTRY("raiseEcsInvalidConfiguration");

    // If there are any alarms already raised, we need to close them first, otherwise the
    // previous alarm will stay visible and this one won't be displayed.

    if (getIsEcsInvalidConfigurationRaised())
    {
        closeEcsInvalidConfiguration();
    }

  	TA_Base_Core::ThreadGuard guard( m_lock );


    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }
        
        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
	    TA_Base_Core::NameValuePair pair1 ( "Detail", detail );
	    descriptionParameters.push_back ( &pair1 );

        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset ( 
            TA_Base_Core::ECSAgentAlarms::EcsInvalidConfiguration, 
            m_cachedConfigInstance.getAgentKey(),	        	// entity key
            m_cachedConfigInstance.getAgentTypeKey(),	    	// entity type key
            descriptionParameters,                                  // parameters
		    m_cachedConfigInstance.getAgentName(),			// entity name
            m_cachedConfigInstance.getAgentLocationKey(),		// region key
            m_cachedConfigInstance.getAgentSubsystemKey(),		// subystem key
			m_cachedConfigInstance.getAssetName(),
			TA_Base_Core::AlarmConstants::defaultTime );

        m_ecsInvalidConfigurationRaised = true;

    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to raise the EcsInvalidConfiguration alarm");
    }
    

    FUNCTION_EXIT;
}

void CtaEcsAlarms::closeEcsInvalidConfiguration(bool isForced)
{
 	FUNCTION_ENTRY("closeEcsInvalidConfiguration");

    // if isForced, then we don't bother checking whether the alarm is raised already
    if (!isForced)
    {
        if (!getIsEcsInvalidConfigurationRaised())
        {
            // Already closed
            return;
        }
    }

  	TA_Base_Core::ThreadGuard guard( m_lock );

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
            TA_Base_Core::ECSAgentAlarms::EcsInvalidConfiguration, 
            m_cachedConfigInstance.getAgentKey(),
            m_cachedConfigInstance.getAgentLocationKey());

        m_ecsInvalidConfigurationRaised = false;

    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to close the EcsInvalidConfiguration alarm");
    }
    

    FUNCTION_EXIT;
}



// ExceptionChecked
void CtaEcsAlarms::raiseEcsZoneEmergencyNormalisation( unsigned long zoneId )
{
 	FUNCTION_ENTRY("raiseEcsZoneEmergencyNormalisation");

  	TA_Base_Core::ThreadGuard guard( m_lock );

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }

        // construct the DescriptionParameters list
	    TA_Base_Core::DescriptionParameters descriptionParameters;
        TA_Base_Core::NameValuePair pair1 ( "EcsLocation", m_cachedMapsInstance.getEcsLocation(zoneId) );
        TA_Base_Core::NameValuePair pair2 ( "Track", m_cachedMapsInstance.getTrack(zoneId) );
	    descriptionParameters.push_back ( &pair1 );
        descriptionParameters.push_back ( &pair2 );

        TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
            TA_Base_Core::ECSAgentAlarms::EcsZoneEmergencyNormalisation, 
            m_cachedConfigInstance.getAgentKey(),	        	// entity key
            m_cachedConfigInstance.getAgentTypeKey(),	    	// entity type key
            descriptionParameters,                                  // parameters
		    m_cachedConfigInstance.getAgentName(),			// entity name
            m_cachedConfigInstance.getAgentLocationKey(),		// region key
            m_cachedConfigInstance.getAgentSubsystemKey(),		// subystem key
			m_cachedConfigInstance.getAssetName(),
			TA_Base_Core::AlarmConstants::defaultTime );
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to raise the EcsZoneEmergencyNormalisation alarm");
    }
    
    FUNCTION_EXIT;
}


// ExceptionChecked
void CtaEcsAlarms::closeEcsZoneEmergencyNormalisation()
{
 	FUNCTION_ENTRY("closeEcsZoneEmergencyNormalisation()");

    TA_Base_Core::ThreadGuard guard( m_lock );

    // Obtain handle to the unique Alarm Helper.  Deliberately using a unique 
    // helper for non-unique alarms to close all alarms of this type.

    try
    {   
        if (m_cachedConfigInstance.getAgentKey() == 0)
        {
            // The agent has not been initialised completely, we can only log this error 
            // below
            throw "error";
        }
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
            TA_Base_Core::ECSAgentAlarms::EcsZoneEmergencyNormalisation, 
            m_cachedConfigInstance.getAgentKey(),
            m_cachedConfigInstance.getAgentLocationKey());

    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to close the EcsZoneEmergencyNormalisation alarms");
    }
    

    FUNCTION_EXIT;
}
