/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/AtsAgentInterface.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"

#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/OperationNotSupportedException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>
#include <sstream>


namespace TA_IRS_App
{

    AtsAgentInterface::AtsAgentInterface( ITrainAgentConfiguration& configuration )
            : m_atsAgent(),
              m_atsEntityKey( 0 ),
              m_atsEntityName()
    {
        FUNCTION_ENTRY( "AtsAgentInterface" );

        // load all the required data from the database using loadAtsObject
        loadAtsObject( configuration );

        FUNCTION_EXIT;
    }


    AtsAgentInterface::~AtsAgentInterface()
    {
        FUNCTION_ENTRY( "~AtsAgentInterface" );
        FUNCTION_EXIT;
    }


    void AtsAgentInterface::sendOa1TableToAts( TA_IRS_Bus::CommonTypes::TrainIdType physicalTrainNumber,
                                               const TA_IRS_Bus::TrainMessageData& oa1 )
    {
        FUNCTION_ENTRY( "sendOa1TableToAts" );

        // convert the data to the corba type
        TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence oa1Corba;
        oa1Corba.length( oa1.size() );

        unsigned long i = 0;

        for ( TA_IRS_Bus::TrainMessageData::const_iterator iter = oa1.begin();
              iter != oa1.end(); ++iter )
        {
            oa1Corba[ i++ ] = *iter;
        }

        try
        {
            CORBA_CALL( m_atsAgent, sendOa1TableToAts, ( physicalTrainNumber, oa1Corba ) );//limin
        }
        catch ( TA_Base_Core::NotAvailableException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::NotAvailableException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::OperationNotSupportedException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::BadParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", e.reason.in() );

            std::ostringstream parameterValue;
            parameterValue << physicalTrainNumber;

            TA_THROW( TA_IRS_Bus::InvalidParameterException( e.reason.in(), "physicalTrainNumber", parameterValue.str() ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::ObjectResolutionException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;
    }


    void AtsAgentInterface::sendAtcTrainStatusReportToAts( TA_IRS_Bus::CommonTypes::TrainIdType physicalTrainNumber,
                                                           const TA_IRS_Bus::TrainMessageData& atcTrainStatus )
    {
        FUNCTION_ENTRY( "sendAtcTrainStatusReportToAts" );

        //convert the data to the corba type
        TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence atcTrainStatusCorba;
        atcTrainStatusCorba.length( atcTrainStatus.size() );

        unsigned long i = 0;

        for ( TA_IRS_Bus::TrainMessageData::const_iterator iter = atcTrainStatus.begin();
              iter != atcTrainStatus.end(); ++iter )
        {
            atcTrainStatusCorba[ i++ ] = *iter;
        }

        try
        {
            CORBA_CALL( m_atsAgent, sendAtcTrainStatusReportToAts, ( physicalTrainNumber, atcTrainStatusCorba ) );//limin
        }
        catch ( TA_Base_Core::NotAvailableException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::NotAvailableException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::OperationNotSupportedException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::BadParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", e.reason.in() );

            std::ostringstream parameterValue;
            parameterValue << physicalTrainNumber;

            TA_THROW( TA_IRS_Bus::InvalidParameterException( e.reason.in(), "physicalTrainNumber", parameterValue.str() ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::ObjectResolutionException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;
    }


    AtsTrainInformationList AtsAgentInterface::getTrainInformationList()
    {
        FUNCTION_ENTRY( "getTrainInformationList" );

        TA_IRS_App::AtsTrainInformationList result;

        try
        {
            // use m_atsAgent to query the ATS agent
            TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList_var trainInfoListVar;//limin
            CORBA_CALL_RETURN( trainInfoListVar, m_atsAgent, getTrainInformationList, () );//limin

            // convert the data from the corba type to the C++ type
            result.reserve( trainInfoListVar->length() );

            for ( unsigned long i = 0; i < trainInfoListVar->length(); ++i )
            {
                AtsTrainInformation cppAtsTrainInfo;

                cppAtsTrainInfo.physicalTrainNumber = trainInfoListVar[i].physicalTrainNumber;
                cppAtsTrainInfo.trainValid = trainInfoListVar[i].trainValid;
                cppAtsTrainInfo.localisation.currentCctvZone = trainInfoListVar[i].localisation.currentCctvZone;
                cppAtsTrainInfo.localisation.currentLocation = trainInfoListVar[i].localisation.currentLocation;
                cppAtsTrainInfo.localisation.isMute = trainInfoListVar[i].localisation.isMute;
                cppAtsTrainInfo.localisation.lastLocation = trainInfoListVar[i].localisation.lastLocation;
                cppAtsTrainInfo.localisation.localisationValid = trainInfoListVar[i].localisation.localisationValid;
                cppAtsTrainInfo.localisation.physicalTrainService = trainInfoListVar[i].localisation.physicalTrainService;

                switch ( trainInfoListVar[i].localisation.track )
                {

                    case TA_IRS_Bus::TrackInnerbound:
                        // fall through

                    case TA_IRS_Bus::TrackCentralInnerbound:
                        cppAtsTrainInfo.localisation.track = TA_IRS_Bus::TrainInformationTypes::AtsTrackInner;
                        break;

                    case TA_IRS_Bus::TrackOuterbound:
                        // fall through

                    case TA_IRS_Bus::TrackCentralOuterbound:
                        cppAtsTrainInfo.localisation.track = TA_IRS_Bus::TrainInformationTypes::AtsTrackOuter;
                        break;

                    case TA_IRS_Bus::TrackUnknown:
                        // fall through

                    default:
                        cppAtsTrainInfo.localisation.track = TA_IRS_Bus::TrainInformationTypes::AtsTrackUnknown;
                        break;
                }

                cppAtsTrainInfo.localisation.upDirection = trainInfoListVar[i].localisation.upDirection;

                result.push_back( cppAtsTrainInfo );
            }
        }
        catch ( TA_Base_Core::NotAvailableException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::NotAvailableException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::OperationNotSupportedException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::ObjectResolutionException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;
        return result;
    }


    bool AtsAgentInterface::inOccDoubleAtsFailure()
    {
        FUNCTION_ENTRY( "inOccDoubleAtsFailure" );

        try
        {
            FUNCTION_EXIT;
            CORBA::Boolean ret;
            CORBA_CALL_RETURN( ret, m_atsAgent, inOccDoubleAtsFailure, () );//limin
            return ret;//limin
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::ObjectResolutionException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        // prevent compiler warnings
        FUNCTION_EXIT;
        return false;
    }


    void AtsAgentInterface::radioInFallback( bool radioFallback )
    {
        FUNCTION_ENTRY( "radioInFallback" );

        try
        {
            CORBA_CALL( m_atsAgent, radioInFallback, ( radioFallback ) );//limin
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::ObjectResolutionException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;
    }


    void AtsAgentInterface::resetAts()
    {
        FUNCTION_ENTRY( "resetAts" );

        try
        {
            CORBA_CALL( m_atsAgent, resetAts, () );//limin
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::ObjectResolutionException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;
    }


    unsigned long AtsAgentInterface::getAtsEntityKey()
    {
        FUNCTION_ENTRY( "getAtsEntityKey" );
        FUNCTION_EXIT;
        return m_atsEntityKey;
    }


    std::string AtsAgentInterface::getAtsAgentName()
    {
        FUNCTION_ENTRY( "getAtsAgentName" );
        FUNCTION_EXIT;
        return m_atsEntityName;
    }


    void AtsAgentInterface::loadAtsObject( ITrainAgentConfiguration& configuration )
    {
        FUNCTION_ENTRY( "loadAtsObject" );

        // get the location key from the configuration object
        unsigned long locationKey = configuration.getLocationKey();

        // load the AtsAgentEntityData for this location (there should only be one)
        TA_Base_Core::IEntityDataList entityList =
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
                TA_Base_Core::AtsAgentEntityData::getStaticType(),
                locationKey );

        // check the number of records

        if ( entityList.size() != 1 )
        {
            for ( TA_Base_Core::IEntityDataList::iterator iter = entityList.begin();
                  entityList.end() != iter; ++iter )
            {
                delete *iter;
                *iter = NULL;
            }

            std::ostringstream error;

            error << "Incorrect configuration, there are "
            << entityList.size()
            << " ats entities configured at this location";

            TA_THROW( TA_Base_Core::FatalConfigurationException( error.str() ) );
        }

        TA_Base_Core::AtsAgentEntityDataPtr atsEntity;

        atsEntity.reset( dynamic_cast< TA_Base_Core::AtsAgentEntityData* >( entityList[0] ) );
        TA_ASSERT( NULL != atsEntity.get(), "Invalid ats entity object" );

        // from this, populate m_atsAgent with the loaded corba name
        // also populate the ats entity key from this data
        m_atsEntityKey = atsEntity->getKey();
        m_atsEntityName = atsEntity->getName();

        m_atsAgent.setCorbaName( atsEntity->getName(), atsEntity->getAgentName() );

        FUNCTION_EXIT;
    }


}// TA_IRS_App
