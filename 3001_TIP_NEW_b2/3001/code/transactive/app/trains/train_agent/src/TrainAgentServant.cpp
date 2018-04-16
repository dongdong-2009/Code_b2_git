/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/trains/train_agent/src/TrainAgentServant.cpp $
 * @author:  Khalil Dowlut
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 15:10:25 $
 * Last modified by:  $Author: builder $
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/trains/train_agent/idl/src/ITrainInformationCorbaDef.h" 

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
//#include "bus/trains/TrainTransactionModel/src/ProtocolException.h"
//#include "bus/trains/TrainTransactionModel/src/TrainBusyException.h"
//#include "bus/trains/TrainTransactionModel/src/TrainTimerException.h"
//#include "bus/trains/TrainTransactionModel/src/InvalidTrainException.h"
//#include "bus/trains/TrainTransactionModel/src/TrainTimeoutException.h"
//#include "bus/trains/TrainTransactionModel/src/AccessDeniedException.h"
//#include "bus/trains/TrainTransactionModel/src/TrainTransmissionFailureException.h"

#include "app/trains/train_agent/src/TrainAgent.h"
#include "app/trains/train_agent/src/TrainAgentDatatypes.h"
#include "app/trains/train_agent/src/TrainAgentServant.h"
//#include "app/trains/train_agent/src/TrainAgentFramework.h"
#include "app/trains/train_agent/src/RadioAccess.h"
class TrainAgent;
namespace 
{
    typedef std::list<TrainAgentDatatypes::TrainInformation>
        TrainInfoList;

    typedef TrainInfoList::const_iterator 
        TrainInfoListConstIter;

    typedef std::list<TrainAgentDatatypes::TrainCommsStateUpdate>
        TrainCommsStateList;

    typedef TrainCommsStateList::const_iterator
        TrainCommsStateConstIter;

    typedef std::list<TrainAgentDatatypes::TrainErrorState>
        TrainErrorStateList;

    typedef TrainErrorStateList::const_iterator
        TrainErrorStateConstIter;

    typedef std::list<unsigned char> 
        TrainIdList;

    const std::string genericErrorMsg("TrainAgentServant failed unexpectedly.");

    //
    // Helper methods
    //

    /**
	  * corbaEnumToCpp
	  * 
	  * Maps a TA_IRS_Bus_4677::IPaPidsTrainCommandsCorbaDef::EFreeTextColour 
      * enum to a TrainAgentDatatypes::EFreeTextColour.
      * Default return value is ColourBlack.
	  * 
	  * @return TrainAgentDatatypes::EFreeTextColour 
	  * @param : const TA_IRS_Bus_4677::IPaPidsTrainCommandsCorbaDef::EFreeTextColour& colour
	  */
    
}   // namespace{}


namespace TA_IRS_App
{


TrainAgentServant::TrainAgentServant(TrainAgent& trainAgent)
:
    m_trainAgent(trainAgent)
{
	try
	{
		std::string agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
		activateServantWithName(agentName);
	}
	catch( const TA_Base_Core::DatabaseException& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
		LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "TrainAgent (name unknown) cannot be registered." );
		throw;
	}
	catch( const TA_Base_Core::DataException& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
		LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "TrainAgent (name unknown) cannot be registered." );
		throw;
	}
	catch( const TA_Base_Core::TransactiveException& e )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
		LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "TrainAgent (name unknown) cannot be registered." );
		throw;
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "TrainAgent (name unknown) cannot be registered." );
		throw;
    }
}

TrainAgentServant::~TrainAgentServant()
{
    FUNCTION_ENTRY("~TrainAgentServant");
	deactivateAndDeleteServant();
    FUNCTION_EXIT;
}

void TrainAgentServant::ReceiveSDSData(const TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef::SDSData& data, CORBA::ULong srcRadioId,CORBA::ULong destRadioId)
{
    FUNCTION_ENTRY("receiveSDSData");

	checkOpeartionMode();

    try
    {
		std::ostringstream SDSRadioId,DEST_SDSReadioId;
        SDSRadioId << srcRadioId;
		DEST_SDSReadioId << destRadioId;
		std::vector<unsigned char> sdsData;
        sdsData.reserve(data.length());

		std::ostringstream dump;
		dump << std::hex;		//dump is only used for log

        for (CORBA::ULong i=0; i < data.length(); ++i)
        {
            sdsData.push_back(data[i]);
			//for debugging purposes
			dump << static_cast<int>(data[i]) << " ";
        }
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"[SDS recv from radio %s to Radio %s][Hex] %s", 
						SDSRadioId.str().c_str(), DEST_SDSReadioId.str().c_str(),
						dump.str().c_str());

		RadioAccess* radioAccess = m_trainAgent.getRadioAccess();
		radioAccess->receiveSDSData(sdsData,SDSRadioId.str(),DEST_SDSReadioId.str());
    }
/*    catch(InvalidParameterException e)
    {
        TA_IRS_Bus::IRecvSDSTrainAgentCorbaDef::InvalidParameterException corbaEx;
        corbaEx.message = e.what();
        throw(corbaEx);
    }*/
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "...", "Unexpected exception in "
                            "TrainAgentServant::receiveSDSData()");
	}

    FUNCTION_EXIT;
}


void TrainAgentServant::checkOpeartionMode()
{
	m_trainAgent.checkOperationMode();
}
}