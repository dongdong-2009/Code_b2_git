/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: PRIV_4669_TRAIN_ECP/CODE_4669/transactive/bus/radio/RadioCallBannerLibrary/src/CallDriverCallEntry.cpp $
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */


#include "bus/banner_pages/call_page_library/src/stdafx.h"
#include "bus/radio/RadioCallBannerLibrary/src/CallDriverCallEntry.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <sstream>
#include <iomanip>


namespace TA_IRS_Bus
{

    const std::string CallDriverCallEntry::DRIVER_ENTRY_TYPE = "Train";
    const std::string CallDriverCallEntry::CALL_STATUS_STRING = "Ringing";
    const std::string CallDriverCallEntry::CALL_INFORMATION_STRING = "Callback";


    CallDriverCallEntry::CallDriverCallEntry( unsigned long id,
                                              const CallTypes::OccCallRequest& occCall,
                                              const TrainInformationTypes::TrainDetails& trainDetails,
											  TrainDriverCallCorbaProxyPtr driverCallProxy)
        : AbstractCallEntry( id,
                             getSource( occCall, trainDetails ),
                             CALL_INFORMATION_STRING ),
          m_occCall( occCall ),
          m_trainDetails( trainDetails ),
		  m_trainDriverCallCORBAProxy(driverCallProxy)
    {
        FUNCTION_ENTRY( "CallDriverCallEntry" );
        FUNCTION_EXIT;
    }


    CallDriverCallEntry::~CallDriverCallEntry()
    {
        FUNCTION_ENTRY( "~CallDriverCallEntry" );
        FUNCTION_EXIT;
    }


    std::string CallDriverCallEntry::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return DRIVER_ENTRY_TYPE;
    }


    std::string CallDriverCallEntry::getCategory() const
    {
        FUNCTION_ENTRY( "getCategory" );
        FUNCTION_EXIT;
        return DRIVER_ENTRY_TYPE;
    }


    std::string CallDriverCallEntry::getType() const
    {
        FUNCTION_ENTRY( "getType" );
        FUNCTION_EXIT;
        return DRIVER_ENTRY_TYPE;
    }


    std::string CallDriverCallEntry::getLocationStr() const
    {
        FUNCTION_ENTRY( "getLocationStr" );

        std::string locationString( "???" );


        if ( 0 != m_trainDetails.currentLocation )
        {
            locationString = LocationCache::getInstance().getLocationByKey( m_trainDetails.currentLocation );
        }
        else if ( 0 != m_trainDetails.communicatingLocation )
        {
            // just need to make sure the location is not set to 'OCC' as this is not an actual location
            // on the mainline, all others such as stations/depot are.
            if ( LocationCache::getInstance().getOccLocationKey() != m_trainDetails.communicatingLocation )
            {
                locationString = LocationCache::getInstance().getLocationByKey( m_trainDetails.communicatingLocation );
            }
        }

        FUNCTION_EXIT;
        return locationString;
    }


    int CallDriverCallEntry::getSeverity() const
    {
        FUNCTION_ENTRY( "getSeverity" );

        if ( true == isEmergency() )
        {
            FUNCTION_EXIT;
            return 2;
        }

        FUNCTION_EXIT;
        return 1;
    }


    std::string CallDriverCallEntry::getStatusStr() const
    {
        FUNCTION_ENTRY( "getStatusStr" );
        FUNCTION_EXIT;
        return CALL_STATUS_STRING;
    }


    time_t CallDriverCallEntry::getCreateTime() const
    {
        FUNCTION_ENTRY( "getCreateTime" );
        FUNCTION_EXIT;
        return m_occCall.timestamp;
    }


    bool CallDriverCallEntry::isEmergency() const
    {
        FUNCTION_ENTRY( "isEmergency" );

        // These calls cant be emergency

        FUNCTION_EXIT;
        return false;
    }


    void CallDriverCallEntry::onActivate()
    {
        FUNCTION_ENTRY( "onActivate" );
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Activating CallDriver Call");
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
		
		try
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"Session Id : %s Train Id : %d Train TSI : %s",sessionId.c_str(),m_trainDetails.trainId,m_trainDetails.primaryTsi.c_str());

			m_trainDriverCallCORBAProxy->answerOccCallRequest (m_trainDetails.trainId,sessionId);
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
						"Exception Thrown during answering driver call");
		}

        FUNCTION_EXIT;
    }

    void CallDriverCallEntry::updateDetails( const CallTypes::OccCallRequest& occCall )
    {
        FUNCTION_ENTRY( "updateDetails( OccCallRequest )" );

        m_occCall = occCall;

        // The train ID hasnt changed
        // and the source cant change based on this update
        // all that can change is the timestamp
        // and that is queried and doesnt need to be updated

        FUNCTION_EXIT;
    }


    void CallDriverCallEntry::updateDetails( const TrainInformationTypes::TrainDetails& trainDetails )
    {
        FUNCTION_ENTRY( "updateDetails( TrainDetails)" );

        m_trainDetails = trainDetails;

        // the source can change if the service number changes
        setSource( getSource( m_occCall, m_trainDetails ) );

        FUNCTION_EXIT;
    }


    std::string CallDriverCallEntry::getSource( const CallTypes::OccCallRequest& occCall,
                                                const TrainInformationTypes::TrainDetails& trainDetails ) const
    {
        FUNCTION_ENTRY( "getSource" );

        std::stringstream source;

        if ( false == trainDetails.serviceNumber.empty() )
        {
            source << trainDetails.serviceNumber;
        }
        else
        {
            source << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( occCall.trainId ) << "/_";
        }

        FUNCTION_EXIT;
        return source.str();
    }

}
